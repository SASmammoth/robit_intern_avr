# AVR 과제 Repository

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민

---

## 1. 개요 (Overview)
본 저장소는 로빛 신입단원 AVR 교육 관련 과제 제출을 위한 공간임.

### 핵심 목표
* 과제에 최선을 다하여 해결하기
* 과도한 AI 사용을 자제하기
* 과제를 제 시간에 제출하기


---

## 2. 저장소 구조 (Repository Structure)
```text
.
├── README.md
└── src
    ├── Day_01
    │   ├── Day01_Task02
    │   │   ├── Day01_Task02.cproj
    │   │   └── main.c
    │   ├── Day01_Task03
    │   │   ├── Day01_Task03.cproj
    │   │   └── main.c
    │   ├── Day_01.atsln
    │   ├── Task_02
    │   └── Task_03
    ├── Day_02
    │   ├── Day02_Task02
    │   │   ├── Day02_Task02.cproj
    │   │   ├── i2c_master.c
    │   │   ├── i2c_master.h
    │   │   ├── liquid_crystal_i2c.c
    │   │   ├── liquid_crystal_i2c.h
    │   │   └── main.c
    │   ├── Day02_Task03
    │   │   ├── Day02_Task03.cproj
    │   │   ├── i2c_master.c
    │   │   ├── i2c_master.h
    │   │   ├── liquid_crystal_i2c.c
    │   │   ├── liquid_crystal_i2c.h
    │   │   └── main.c
    │   ├── Day_02.atsln
    │   ├── LCD_Text.c
    │   └── LCD_Text.h
    └── Day_03


```

---

## 5. 참고 (Note)

프로젝트 세팅 ppt 파일에는 프로젝트 마다 솔루션이 할당되어 있다. 하지만 일자별로 프로젝트를 깔끔하게 보기 위해 날짜별로 솔루션을 묶었다. ppt의 강조사항에 솔루션에 관한 것은 없기 때문에 이렇게 하였다.


### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
