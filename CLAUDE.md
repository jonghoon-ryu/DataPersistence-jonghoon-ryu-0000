# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

JSON data 를 parsing 하고 CRUD 를 적용시키는 프로그램 및 테스트 코드를 만든다.
Json.h 및 Json.cpp 를 참고해서 만들면 되고, 항목(스키마/필드 구성)은 중요하지 않음.
TC 까지 만든 후 사용자 승인을 기다릴 것.

1. json 데이터에 CRUD 가 발생할때 디렉토리에 저장한다. 디렉토리 이름은 jsonData 라고 해.
2. 현재 시간을 읽을 수 있는 함수를 만든다. 초 단위로 읽을 수 있어야 함. 리턴 방식은 1900-08-21 01:23:34 식으로 함.
