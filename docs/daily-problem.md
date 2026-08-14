# 🎲 오늘의 문제 — 내 레포에 가져다 쓰기

프로그래머스 문제를 **매일 자정(KST)에 자동으로 몇 개 뽑아 README에 띄워주는** 기능입니다.
공개 API만 쓰고 별도 토큰·비밀값이 필요 없어서, 아래 4단계면 어느 레포에서도 그대로 동작합니다.

> ℹ️ **이 가이드는 '오늘의 문제' 추천 기능만 다룹니다.** 원본 레포의 **잔디 히트맵**은
> `streak-grass.mjs` 기반의 **별개 기능**이고, 원본 [`grass.yml`](../.github/workflows/grass.yml)은 이 둘을 한 워크플로에서 함께 돌립니다.
> 오늘의 문제만 쓸 거면 아래 워크플로를 그대로 쓰세요 — **`grass.yml`을 통째로 복사하면 안 됩니다**
> (잔디 스텝이 `streak-grass.mjs` 없이 실패). 잔디까지 원하면 `streak-grass.mjs`도 복사하고 해당 스텝을 따로 추가하면 됩니다.

## 동작 개요

- 프로그래머스 공개 API(`/api/v2/school/challenges`)에서 문제 목록을 가져옵니다.
- 지정한 레벨(기본 2·3·4)에서 **각 1문제씩** 뽑아 **순서를 랜덤하게 섞습니다** (표시에는 난이도를 노출하지 않음).
- 특정 레벨에 안 푼 문제가 없으면 그 레벨은 건너뛰고, 전부 소진되면 실행이 실패합니다.
- **이미 푼 문제는 제외** — [BaekjoonHub](https://github.com/BaekjoonHub/BaekjoonHub)가 만든
  `프로그래머스/{레벨}/` 안의 `{id}.` 로 시작하는 폴더명에서 id를 뽑아 후보에서 뺍니다.
- **언어 필터**(선택) — `PICK_LANG`을 지정하면 그 언어로 **제출 가능한 문제만** 추립니다
  (예: 파이썬 유저 → SQL 전용 문제 등 자동 제외). 비우면 전체.
- **날짜 시드** 고정이라 같은 날은 몇 번 실행돼도 결과가 같습니다 (커밋 때마다 흔들리지 않음).
- **리롤** 지원 — 마음에 안 들면 수동으로 다시 뽑을 수 있고, 그 결과가 그날 내내 유지됩니다.

## 설치 (4단계)

1. **`pick-problem.mjs` 복사** — [`.github/scripts/pick-problem.mjs`](../.github/scripts/pick-problem.mjs)를 같은 경로에 그대로 넣습니다.

2. **워크플로 추가** — 아래 내용을 `.github/workflows/today.yml`로 저장하면 끝입니다.
   (이미 돌리는 워크플로가 있다면 `오늘의 문제 뽑기`·`변경분 커밋` **두 스텝만** 그쪽으로 옮겨도 됩니다.)

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

4. **끝.** 다음 자정에 첫 문제가 채워지고, Actions에서 수동 실행하면 즉시 채워집니다.

## 설정 (환경변수 — 전부 선택)

| 변수 | 기본값 | 설명 |
|---|---|---|
| `PICK_LEVELS` | `2,3,4` | 뽑을 레벨 목록. 각 레벨에서 1문제씩 뽑아 순서를 섞음 |
| `PICK_LANG` | (없음) | 지정 시 그 언어로 제출 가능한 문제만. `c`·`cpp`·`csharp`·`go`·`java`·`javascript`·`kotlin`·`python3`·`ruby`·`scala`·`swift`·`mysql`·`oracle` |
| `SOLVED_DIR` | `프로그래머스` | 푼 문제 폴더 (BaekjoonHub 기본값) |
| `README_PATH` | `README.md` | 갱신할 파일 경로 |
| `REROLL` | (없음) | `1`/`true`면 그날 문제를 다시 뽑음 |

예: 레벨 1·2·3에서, 파이썬으로 풀 수 있는 문제만 →
```yaml
env:
  PICK_LEVELS: "1,2,3"
  PICK_LANG: "python3"
```

## 리롤 방법

Actions 탭 → 워크플로 선택 → **Run workflow** → `reroll` 체크 후 실행.
`.github/.today-salt` 파일에 그날의 리롤 횟수가 저장돼, 이후 자동 재실행돼도 리롤 결과가 유지됩니다.

## 주의

- 프로그래머스 **비공식** 공개 엔드포인트라, 사이트 개편 시 응답 형식이 바뀔 수 있습니다.
- 과도한 호출은 피하세요 (이 스크립트는 하루 몇 번 수준).

## 라이선스

[MIT](../LICENSE) — 자유롭게 가져다 쓰고 수정하셔도 됩니다. 저작권 고지만 유지해 주세요.
