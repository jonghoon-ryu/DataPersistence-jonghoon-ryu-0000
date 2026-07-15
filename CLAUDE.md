# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

JSON data 를 parsing 하고 CRUD 를 적용시키는 프로그램 및 테스트 코드를 만든다.
Json.h 및 Json.cpp 를 참고해서 만들면 되고, 항목(스키마/필드 구성)은 중요하지 않음.
TC 까지 만든 후 사용자 승인을 기다릴 것.

1. json 데이터에 CRUD 가 발생할때 디렉토리에 저장한다. 디렉토리 이름은 jsonData 라고 해.
2. 현재 시간을 읽을 수 있는 함수를 만든다. 초 단위로 읽을 수 있어야 함. 리턴 방식은 1900-08-21 01:23:34 식으로 함.

## 데이터 형식 (docs/project.pdf 기준: 반도체 시료 생산주문관리 시스템)

이 시스템에서 사용하는 데이터는 아래 3가지이며, 각 데이터는 jsonData 밑의 서로 다른
서브 디렉토리에 저장한다 (jsonData/samples, jsonData/orders, jsonData/productionQueue).

1. 시료 (Sample) — jsonData/samples
   - 시료 ID, 이름, 평균 생산시간, 수율, 현재 재고

2. 주문 (Order) — jsonData/orders
   - 주문번호: `YYYY-MM-DD-HH-MM-SS-시료ID-SEQ` 형식 (예: 2026-07-15-11-08-05-S-001-000)
   - 시료 ID, 고객명, 주문 수량, 상태 (RESERVED/CONFIRMED/PRODUCING/REJECTED/RELEASE)

3. 생산 큐 (Production Queue) — jsonData/productionQueue
   - 주문번호, 시료명(또는 시료 ID), 주문량, 재고, 총 생산 시간, 예상 완료 시각
   - FIFO(선입선출) 구조로 만들어서 주문이 들어온 순서대로 확인 가능해야 함
   - 진행률, 실생산량, 부족분은 포함하지 않음
