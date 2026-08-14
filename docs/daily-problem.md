# 🎲 오늘의 문제 — 내 레포에 가져다 쓰기

프로그래머스 문제를 **매일 자정(KST)에 자동으로 몇 개 뽑아 README에 띄워주는** 기능입니다.
공개 API만 쓰고 별도 토큰·비밀값이 필요 없어서, 아래 4단계면 어느 레포에서도 그대로 동작합니다.

## 동작 개요

- 프로그래머스 공개 API(`/api/v2/school/challenges`)에서 문제 목록을 가져옵니다.
- 지정한 레벨(기본 2·3·4)에서 **각 1문제씩** 뽑아 **순서를 랜덤하게 섞습니다** (표시에는 난이도를 노출하지 않음).
- **이미 푼 문제는 제외** — [BaekjoonHub](https://github.com/BaekjoonHub/BaekjoonHub)가 만든
  `프로그래머스/{레벨}/{id}. {제목}` 폴더를 읽어 후보에서 뺍니다.
- **날짜 시드** 고정이라 같은 날은 몇 번 실행돼도 결과가 같습니다 (커밋 때마다 흔들리지 않음).
- **리롤** 지원 — 마음에 안 들면 수동으로 다시 뽑을 수 있고, 그 결과가 그날 내내 유지됩니다.

## 설치 (4단계)

1. **파일 2개 복사**
   - [`.github/scripts/pick-problem.mjs`](../.github/scripts/pick-problem.mjs)
   - 워크플로: 이미 GitHub Actions로 뭔가 돌리고 있으면 그 워크플로에
     아래 스텝만 추가하고, 없으면 [`.github/workflows/grass.yml`](../.github/workflows/grass.yml)을 참고해 새로 만드세요.

   ```yaml
   - uses: actions/setup-node@v4
     with: { node-version: '20' }
   - name: 오늘의 문제 뽑기
     env:
       REROLL: ${{ inputs.reroll }}   # 리롤 버튼을 쓸 때만 필요
     run: node .github/scripts/pick-problem.mjs
   ```

2. **README에 마커 삽입** — 문제를 띄우고 싶은 위치에 아래 두 줄을 넣습니다.

   ```markdown
   <!-- TODAY:START -->
   <!-- TODAY:END -->
   ```

3. **워크플로 권한/트리거 확인**
   - `permissions: { contents: write }` (커밋 push용)
   - `schedule: - cron: '0 15 * * *'` → 매일 00:00 KST 자동 갱신
   - 리롤 버튼을 쓰려면 `workflow_dispatch.inputs.reroll` (type: boolean) 추가
   - 커밋 스텝에서 `README.md`와 `.github/.today-salt`를 함께 add/commit/push

4. **끝.** 다음 자정에 첫 문제가 채워지고, Actions에서 수동 실행하면 즉시 채워집니다.

## 설정 (환경변수 — 전부 선택)

| 변수 | 기본값 | 설명 |
|---|---|---|
| `PICK_LEVELS` | `2,3,4` | 뽑을 레벨 목록. 각 레벨에서 1문제씩 뽑아 순서를 섞음 |
| `SOLVED_DIR` | `프로그래머스` | 푼 문제 폴더 (BaekjoonHub 기본값) |
| `README_PATH` | `README.md` | 갱신할 파일 경로 |
| `REROLL` | (없음) | `1`/`true`면 그날 문제를 다시 뽑음 |

예: 레벨 1·2·3에서 하나씩 →
```yaml
env:
  PICK_LEVELS: "1,2,3"
```

## 리롤 방법

Actions 탭 → 워크플로 선택 → **Run workflow** → `reroll` 체크 후 실행.
`.github/.today-salt` 파일에 그날의 리롤 횟수가 저장돼, 이후 자동 재실행돼도 리롤 결과가 유지됩니다.

## 주의

- 프로그래머스 **비공식** 공개 엔드포인트라, 사이트 개편 시 응답 형식이 바뀔 수 있습니다.
- 과도한 호출은 피하세요 (이 스크립트는 하루 몇 번 수준).
