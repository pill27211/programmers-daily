# 🎲🌱 가져다 쓰기 가이드 — 오늘의 문제 & 잔디

이 레포의 두 자동화 기능을 **내 레포에도** 얹는 방법입니다. 공개 API와 git 히스토리만 쓰고,
별도 토큰·비밀값이 필요 없어서 어느 레포에서도 그대로 동작합니다.

두 기능은 **서로 독립적**입니다 — 원하는 것만 골라 넣으세요.

- 🎲 **오늘의 문제** — 매일 자정(KST) 프로그래머스 문제 몇 개를 뽑아 README에 표시
- 🌱 **잔디(스트릭)** — 이 레포의 **문제 풀이 커밋만** 세어 GitHub 스타일 히트맵 SVG 생성

> 아래 세 가지 중 본인에게 맞는 것 하나만 펼쳐서 그대로 따라오면 됩니다.
> (원본 [`grass.yml`](../.github/workflows/grass.yml)은 둘을 한 워크플로에서 함께 돌리는 "둘 다" 구성입니다.
> 하나만 쓸 거면 그걸 통째로 복사하지 말고 아래 해당 블록을 쓰세요.)

---

## 🎲 오늘의 문제만 추가

<details>
<summary><b>펼쳐서 따라하기</b></summary>

**동작** — 프로그래머스 공개 API에서 지정 레벨(기본 2·3·4)마다 안 푼 문제를 1개씩 뽑아
순서를 섞어 README에 띄웁니다. 날짜 시드 고정이라 같은 날은 몇 번 실행돼도 결과가 같고, 리롤로 다시 뽑을 수 있습니다.

1. **스크립트 복사** — [`.github/scripts/pick-problem.mjs`](../.github/scripts/pick-problem.mjs)를 같은 경로에 넣습니다.

2. **워크플로 추가** — 아래를 `.github/workflows/today.yml`로 저장합니다.

   ```yaml
   name: 오늘의 문제

   on:
     schedule:
       - cron: '0 15 * * *'      # 매일 00:00 KST
     workflow_dispatch:           # 수동 실행 + 리롤 버튼
       inputs:
         reroll:
           description: '오늘의 문제 다시 뽑기'
           type: boolean
           default: false

   permissions:
     contents: write             # README 커밋 push용

   jobs:
     update:
       runs-on: ubuntu-latest
       steps:
         - uses: actions/checkout@v4
         - uses: actions/setup-node@v4
           with: { node-version: '20' }

         - name: 오늘의 문제 뽑기
           env:
             REROLL: ${{ inputs.reroll }}
             # PICK_LANG: python3     # (선택) 특정 언어로 풀 수 있는 문제만 — 아래 '설정' 참고
           run: node .github/scripts/pick-problem.mjs

         - name: 변경분 커밋
           run: |
             git config user.name "github-actions[bot]"
             git config user.email "41898282+github-actions[bot]@users.noreply.github.com"
             git add README.md .github/.today-salt
             if git diff --cached --quiet; then
               echo "변경 없음"
             else
               git commit -m "chore: 오늘의 문제 갱신 [skip ci]"
               git push
             fi
   ```

3. **README에 마커 삽입** — 문제를 띄우고 싶은 위치에 아래 두 줄을 넣습니다.

   ```markdown
   <!-- TODAY:START -->
   <!-- TODAY:END -->
   ```

4. **끝.** 이 워크플로는 자정(cron)·수동 실행만 트리거라, **셋업 직후엔 마커 사이가 비어 있습니다.**
   다음 자정에 자동으로 채워지고, 바로 보고 싶으면 **Actions 탭 → 오늘의 문제 → Run workflow**로 즉시 채울 수 있습니다.

</details>

---

## 🌱 잔디(스트릭)만 추가

<details>
<summary><b>펼쳐서 따라하기</b></summary>

**동작** — git 히스토리에서 `SOLVED_DIR`(기본 `프로그래머스`) 하위에 **파일이 추가된 커밋만** 날짜별로 세어
라이트/다크 히트맵 SVG를 만듭니다. README·문서·봇 커밋 등은 카운트되지 않아, **실제 풀이 없으면 스트릭이 유지되지 않습니다.**

1. **스크립트 복사** — [`.github/scripts/streak-grass.mjs`](../.github/scripts/streak-grass.mjs)를 같은 경로에 넣습니다.

2. **워크플로 추가** — 아래를 `.github/workflows/grass.yml`로 저장합니다.

   ```yaml
   name: 잔디 갱신

   on:
     push:
       branches: [main]
       paths-ignore:
         - 'assets/**'           # 봇이 만든 SVG 커밋으로 재실행되지 않게
         - 'README.md'
     schedule:
       - cron: '0 15 * * *'      # 매일 00:00 KST
     workflow_dispatch:

   permissions:
     contents: write

   concurrency:
     group: grass
     cancel-in-progress: true

   jobs:
     update:
       runs-on: ubuntu-latest
       steps:
         - uses: actions/checkout@v4
           with: { fetch-depth: 0 }   # 스트릭 계산에 전체 커밋 히스토리 필요

         - uses: actions/setup-node@v4
           with: { node-version: '20' }

         - name: 잔디 SVG 생성
           # env:                     # (선택) 풀이 폴더가 기본(프로그래머스)과 다르면
           #   SOLVED_DIR: 백준
           run: node .github/scripts/streak-grass.mjs

         - name: 변경분 커밋 (+이미지 캐시 무효화)
           run: |
             git config user.name "github-actions[bot]"
             git config user.email "41898282+github-actions[bot]@users.noreply.github.com"
             # SVG가 바뀐 경우에만 README 이미지 주소의 ?v= 갱신 → GitHub 캐시 강제 무효화
             if ! git diff --quiet -- assets/heatmap-dark.svg assets/heatmap-light.svg; then
               V=$(date +%s)
               sed -i -E "s#(assets/heatmap-(dark|light)\.svg)(\?v=[0-9]+)?#\1?v=$V#g" README.md
             fi
             git add -A
             if git diff --cached --quiet; then
               echo "변경 없음"
             else
               git commit -m "chore: 잔디 갱신 [skip ci]"
               git push
             fi
   ```

3. **README에 히트맵 삽입** — 잔디를 띄우고 싶은 위치에 아래 블록을 넣습니다.
   (라이트/다크 테마에 맞춰 자동으로 골라 보여줍니다.)

   ```markdown
   <picture>
     <source media="(prefers-color-scheme: dark)" srcset="assets/heatmap-dark.svg">
     <img alt="이 저장소 풀이 잔디" src="assets/heatmap-light.svg">
   </picture>
   ```

4. **끝.** 이 워크플로는 `push` 트리거가 있어, **셋업 커밋을 올리면 곧바로(≈1분) 실행돼** SVG가 생성됩니다.
   생성 직전 잠깐은 `assets/heatmap-*.svg`가 아직 없어 **깨진 이미지**로 보일 수 있고, 실행이 끝나면 정상 렌더됩니다.
   이후엔 자정 또는 풀이 커밋 push마다 자동 갱신됩니다. (Actions에서 수동 실행도 가능.)

</details>

---

## 🎲 + 🌱 둘 다 추가 (원본 레포 구성)

<details>
<summary><b>펼쳐서 따라하기</b></summary>

한 워크플로에서 잔디와 오늘의 문제를 함께 갱신합니다. 위 두 스크립트를 모두 복사한 뒤 워크플로 하나만 두면 됩니다.

1. **스크립트 2개 복사**
   - [`.github/scripts/pick-problem.mjs`](../.github/scripts/pick-problem.mjs)
   - [`.github/scripts/streak-grass.mjs`](../.github/scripts/streak-grass.mjs)

2. **워크플로 추가** — 아래를 `.github/workflows/grass.yml`로 저장합니다.

   ```yaml
   name: 잔디 갱신

   on:
     push:
       branches: [main]
       paths-ignore:
         - 'assets/**'
         - 'README.md'
     schedule:
       - cron: '0 15 * * *'      # 매일 00:00 KST (잔디 + 오늘의 문제 함께 갱신)
     workflow_dispatch:
       inputs:
         reroll:
           description: '오늘의 문제 다시 뽑기'
           type: boolean
           default: false

   permissions:
     contents: write

   concurrency:
     group: grass
     cancel-in-progress: true

   jobs:
     update:
       runs-on: ubuntu-latest
       steps:
         - uses: actions/checkout@v4
           with: { fetch-depth: 0 }   # 스트릭 계산에 전체 커밋 히스토리 필요

         - uses: actions/setup-node@v4
           with: { node-version: '20' }

         - name: 잔디 SVG 생성
           run: node .github/scripts/streak-grass.mjs

         - name: 오늘의 문제 뽑기
           env:
             REROLL: ${{ inputs.reroll }}
             # PICK_LANG: python3     # (선택) 특정 언어로 풀 수 있는 문제만
           run: node .github/scripts/pick-problem.mjs

         - name: 변경분 커밋 (+이미지 캐시 무효화)
           run: |
             git config user.name "github-actions[bot]"
             git config user.email "41898282+github-actions[bot]@users.noreply.github.com"
             if ! git diff --quiet -- assets/heatmap-dark.svg assets/heatmap-light.svg; then
               V=$(date +%s)
               sed -i -E "s#(assets/heatmap-(dark|light)\.svg)(\?v=[0-9]+)?#\1?v=$V#g" README.md
             fi
             git add -A
             if git diff --cached --quiet; then
               echo "변경 없음"
             else
               git commit -m "chore: 잔디·오늘의 문제 갱신 [skip ci]"
               git push
             fi
   ```

3. **README에 마커 + 히트맵 둘 다 삽입**

   ```markdown
   <!-- TODAY:START -->
   <!-- TODAY:END -->

   <picture>
     <source media="(prefers-color-scheme: dark)" srcset="assets/heatmap-dark.svg">
     <img alt="이 저장소 풀이 잔디" src="assets/heatmap-light.svg">
   </picture>
   ```

4. **끝.** `push` 트리거가 있어 **셋업 커밋 직후(≈1분) 잔디 SVG와 오늘의 문제가 함께** 채워집니다.
   (잔디는 생성 직전 잠깐 깨진 이미지로 보일 수 있음.) 이후 자정·풀이 커밋마다 자동 갱신됩니다.

</details>

---

## 설정 (환경변수 — 전부 선택)

| 변수 | 적용 | 기본값 | 설명 |
|---|---|---|---|
| `PICK_LEVELS` | 🎲 | `2,3,4` | 뽑을 레벨 목록. 각 레벨에서 1문제씩 뽑아 순서를 섞음 |
| `PICK_LANG` | 🎲 | (없음) | 지정 시 그 언어로 제출 가능한 문제만. `c`·`cpp`·`csharp`·`go`·`java`·`javascript`·`kotlin`·`python3`·`ruby`·`scala`·`swift`·`mysql`·`oracle` |
| `README_PATH` | 🎲 | `README.md` | 갱신할 파일 경로 |
| `REROLL` | 🎲 | (없음) | `1`/`true`면 그날 문제를 다시 뽑음 |
| `SOLVED_DIR` | 🎲🌱 | `프로그래머스` | 푼 문제 폴더 (BaekjoonHub 기본값). 🎲는 후보 제외에, 🌱는 커밋 카운트에 사용 |

예: 레벨 1·2·3에서, 파이썬으로 풀 수 있는 문제만 →
```yaml
env:
  PICK_LEVELS: "1,2,3"
  PICK_LANG: "python3"
```

## 리롤 방법 (🎲)

Actions 탭 → 워크플로 선택 → **Run workflow** → `reroll` 체크 후 실행.
`.github/.today-salt` 파일에 그날의 리롤 횟수가 저장돼, 이후 자동 재실행돼도 리롤 결과가 유지됩니다.

## 주의

- 🎲는 프로그래머스 **비공식** 공개 엔드포인트를 씁니다 — 사이트 개편 시 응답 형식이 바뀔 수 있습니다.
- 과도한 호출은 피하세요 (이 스크립트는 하루 몇 번 수준).
- 🌱는 `SOLVED_DIR` 하위에 **파일이 추가된 커밋**만 셉니다. BaekjoonHub처럼 풀이를 새 폴더/파일로 커밋하는 방식과 맞물려 동작합니다.

## 라이선스 · 기여

[MIT](../LICENSE) — 자유롭게 가져다 쓰고 수정하셔도 됩니다 (저작권 고지만 유지해 주세요).
개선 아이디어·버그 제보·기능 추가 등 [원본 저장소](https://github.com/pill27211/programmers-daily)로의 **기여(Issue·PR)도 언제든 환영합니다.**
