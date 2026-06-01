#include <windows.h>  
#include <sqlext.h>  
#include <iostream>

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
    SQLSMALLINT iRec = 0;
    SQLINTEGER iError;
    WCHAR wszMessage[1000];
    WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
    if (RetCode == SQL_INVALID_HANDLE) {
        fwprintf(stderr, L"Invalid handle!\n");
        return;
    }
    while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
        (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
        if (wcsncmp(wszState, L"01004", 5)) {
            fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
        }
    }
}

int main()
{
    setlocale(LC_ALL, "korean");
    std::wcout.imbue(std::locale("korean"));

    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;
    SQLRETURN retcode;

    // 환경 핸들 할당
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

        // 연결 핸들 할당
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

                // DB 연결
                retcode = SQLConnectW(hdbc, (SQLWCHAR*)L"GSP Project", SQL_NTS, (SQLWCHAR*)L"user_id", 0, (SQLWCHAR*)NULL, 0);

                // 명령 핸들 할당
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

                    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

                        std::cout << "connected\n";

                        // [변경점] 복잡한 프로시저 대신, UserInfo 테이블에서 user_id만 싹 긁어오는 기본 쿼리문 적용
                        SQLWCHAR* szSql = (SQLWCHAR*)L"SELECT user_id FROM UserInfo";

                        retcode = SQLExecDirectW(hstmt, szSql, SQL_NTS);

                        // 쿼리문 실행 성공 여부 검증
                        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

                            int nId = 0;
                            SQLLEN cbId = 0;

                            // 가져온 결과셋의 1번째 컬럼(user_id)을 nId 변수에 바인딩
                            SQLBindCol(hstmt, 1, SQL_C_LONG, &nId, 0, &cbId);

                            std::wcout << L"\n===== UserInfo 테이블 전체 user_id 데이터 출력 =====" << std::endl;

                            int nRowCount = 0;

                            // 루프 돌면서 테이블에 저장된 유저 ID 값들을 하나씩 전부 출력
                            while (SQLFetch(hstmt) == SQL_SUCCESS) {
                                nRowCount++;
                                std::wcout << L"[" << nRowCount << L"] 등록된 유저 ID: " << nId << std::endl;
                            }

                            if (nRowCount == 0) {
                                std::wcout << L"UserInfo 테이블에 데이터가 단 하나도 없습니다!" << std::endl;
                            }
                            std::wcout << L"==================================================\n" << std::endl;
                        }
                        else {
                            std::cout << "쿼리 실행 실패!\n";
                            HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
                        }
                    }

                    // 자원 해제 순서 정렬
                    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
                    SQLDisconnect(hdbc);
                }
                else {
                    HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
                }

                SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
            }
        }
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
    }
    return 0;
}