// 프로그래머스 공개 API에서 '오늘의 문제'를 여러 개 뽑아 README의
// <!-- TODAY:START --> ~ <!-- TODAY:END --> 구간을 갱신한다.
//
// - 난이도: 레벨별 가중 랜덤 (기본 2·3 주류, 4는 낮게 — LEVELS_WEIGHTS)
// - 이미 푼 문제({SOLVED_DIR}/{레벨}/{id}. {제목} 폴더)는 후보에서 제외
// - KST 날짜로 시드를 고정 → 같은 날은 몇 번 실행해도 같은 문제 (churn 방지)
// - 리롤: REROLL=1 이면 그날의 salt를 +1 해 새로 뽑고, 그 값을 파일로 저장해
//         이후 (커밋 push 등으로) 재실행돼도 리롤 결과가 유지된다.
//
// 설정(환경변수, 전부 선택):
//   LEVELS_WEIGHTS  기본 "2:40,3:40,4:20"  (레벨:가중치, 쉼표 구분)
//   PICK_COUNT      기본 "3"               (하루에 뽑을 개수)
//   SOLVED_DIR      기본 "프로그래머스"      (BaekjoonHub 풀이 폴더)
//   README_PATH     기본 "README.md"
//   REROLL          "1"/"true" 면 다시 뽑기
//
// 실행: node .github/scripts/pick-problem.mjs
import { readdirSync, statSync, readFileSync, writeFileSync, existsSync } from 'node:fs';
import { join } from 'node:path';

const TZ = 'Asia/Seoul';
const API = 'https://school.programmers.co.kr/api/v2/school/challenges/';
const README = process.env.README_PATH || 'README.md';
const SOLVED_DIR = process.env.SOLVED_DIR || '프로그래머스';
const SALT_FILE = '.github/.today-salt';
const COUNT = Math.max(1, parseInt(process.env.PICK_COUNT || '3', 10) || 3);
const WEIGHTS = parseWeights(process.env.LEVELS_WEIGHTS || '2:40,3:40,4:20');

function parseWeights(spec) {
  const w = {};
  for (const pair of spec.split(',')) {
    const [lv, wt] = pair.split(':').map((s) => s.trim());
    if (lv) w[Number(lv)] = Number(wt) || 0;
  }
  return w;
}

// ---- 날짜 시드 PRNG (mulberry32) ----
function today() {
  return new Date().toLocaleDateString('en-CA', { timeZone: TZ }); // YYYY-MM-DD
}
function hashSeed(str) {
  let h = 2166136261 >>> 0;
  for (const ch of str) { h ^= ch.charCodeAt(0); h = Math.imul(h, 16777619); }
  return h >>> 0;
}
function mulberry32(a) {
  return function () {
    a |= 0; a = (a + 0x6D2B79F5) | 0;
    let t = Math.imul(a ^ (a >>> 15), 1 | a);
    t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

// ---- 리롤 salt (오늘 날짜 기준으로만 유효) ----
function loadSalt(date) {
  try {
    const [d, n] = readFileSync(SALT_FILE, 'utf8').trim().split(':');
    if (d === date) return parseInt(n, 10) || 0;
  } catch { /* 파일 없음 */ }
  return 0;
}
function resolveSalt(date) {
  let salt = loadSalt(date);
  const reroll = String(process.env.REROLL || '').toLowerCase();
  if (['1', 'true', 'yes', 'on'].includes(reroll)) salt += 1;
  writeFileSync(SALT_FILE, `${date}:${salt}\n`);
  return salt;
}

// ---- 이미 푼 문제 id 수집 ----
function solvedIds() {
  const set = new Set();
  if (!existsSync(SOLVED_DIR)) return set;
  for (const lvl of readdirSync(SOLVED_DIR)) {
    const dir = join(SOLVED_DIR, lvl);
    if (!statSync(dir).isDirectory()) continue;
    for (const name of readdirSync(dir)) {
      const m = name.match(/^(\d+)\./); // "159993. 미로 탈출" → 159993
      if (m) set.add(Number(m[1]));
    }
  }
  return set;
}

// ---- API에서 특정 레벨의 전체 문제 목록 (페이지네이션) ----
async function fetchLevel(level) {
  const all = [];
  let page = 1, totalPages = 1;
  do {
    const url = `${API}?perPage=100&levels%5B%5D=${level}&order=recent&search=&page=${page}`;
    const res = await fetch(url, {
      headers: { 'User-Agent': 'programmers-daily-bot', 'Accept': 'application/json' },
    });
    if (!res.ok) throw new Error(`API ${res.status} (level ${level}, page ${page})`);
    const data = await res.json();
    for (const p of data.result ?? []) all.push({ ...p, level });
    totalPages = data.totalPages ?? 1;
    page++;
  } while (page <= totalPages);
  return all;
}

// ---- 가중치 기반 비복원 추출 (distinct k개) ----
function weightedSample(items, weightOf, k, prng) {
  const pool = items.slice();
  const out = [];
  while (out.length < k && pool.length) {
    const total = pool.reduce((s, it) => s + weightOf(it), 0);
    let r = prng() * total, idx = 0;
    for (; idx < pool.length; idx++) { if ((r -= weightOf(pool[idx])) < 0) break; }
    if (idx >= pool.length) idx = pool.length - 1;
    out.push(pool.splice(idx, 1)[0]);
  }
  return out;
}

function renderBlock(date, picks) {
  const lines = picks.map((p, i) => {
    const url = `https://school.programmers.co.kr/learn/courses/30/lessons/${p.id}`;
    return `${i + 1}. [${p.title}](${url}) — \`Lv.${p.level}\` · 정답률 ${p.acceptanceRate}% · ${p.partTitle}`;
  });
  return (
    `**오늘의 문제 · ${date}** — 아래 중 하나 골라 풀면 됩니다 🎯\n\n` +
    lines.join('\n') +
    `\n\n<sub>매일 자정(KST) 자동 갱신 · 다시 뽑기: Actions → **잔디 갱신** → Run workflow(reroll 체크)</sub>`
  );
}

async function main() {
  const date = today();
  const salt = resolveSalt(date);
  const prng = mulberry32(hashSeed(`${date}:${salt}`));
  const solved = solvedIds();

  // 가중치에 등록된 모든 레벨의 안 푼 문제를 모아 하나의 풀로
  const levels = Object.keys(WEIGHTS).map(Number).filter((l) => WEIGHTS[l] > 0);
  const lists = await Promise.all(levels.map(fetchLevel));
  const candidates = lists.flat().filter((p) => !solved.has(p.id));
  if (!candidates.length) throw new Error(`안 푼 문제를 찾지 못했습니다 (레벨 ${levels.join(',')} 전부 풀었을 수도).`);

  const picks = weightedSample(candidates, (p) => WEIGHTS[p.level] || 1, COUNT, prng);

  const md = readFileSync(README, 'utf8');
  const re = /(<!-- TODAY:START -->)[\s\S]*?(<!-- TODAY:END -->)/;
  if (!re.test(md)) throw new Error('README에 <!-- TODAY:START --> / <!-- TODAY:END --> 마커가 없습니다.');
  writeFileSync(README, md.replace(re, `$1\n${renderBlock(date, picks)}\n$2`));

  console.log(`오늘의 문제 ${picks.length}개 (salt=${salt}):`);
  for (const p of picks) console.log(`  [Lv.${p.level}] ${p.title}`);
}

main().catch((e) => { console.error(e.message); process.exit(1); });
