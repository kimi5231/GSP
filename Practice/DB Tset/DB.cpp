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
        // Hide data truncated..
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

    SQLCHAR* OutConnStr = (SQLCHAR*)malloc(255);
    SQLSMALLINT* OutConnStrLen = (SQLSMALLINT*)malloc(255);

    // Allocate environment handle  
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

    // Set the ODBC version environment attribute  
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

        // Allocate connection handle  
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

            // Set login timeout to 5 seconds  
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

                retcode = SQLConnectW(hdbc, (SQLWCHAR*)L"GSP", SQL_NTS, (SQLWCHAR*)NULL, 0, (SQLWCHAR*)NULL, 0);

                // Allocate statement handle  
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

                    // Process data  
                    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

                        std::cout << "connected\n";

                        SQLWCHAR* szSql = (SQLWCHAR*)L"select_highlevel 10";

                        retcode = SQLExecDirectW(hstmt, szSql, SQL_NTS);

                        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

                            int nId = 0;
                            WCHAR wszName[50] = { 0, };
                            int nLevel = 0;
                            SQLLEN cbId = 0, cbName = 0, cbLevel = 0;

                            // 4. 컬럼 바인딩 (1번: id, 2번: name, 3번: level)
                            SQLBindCol(hstmt, 1, SQL_C_LONG, &nId, 0, &cbId);
                            SQLBindCol(hstmt, 2, SQL_C_WCHAR, wszName, sizeof(wszName), &cbName);
                            SQLBindCol(hstmt, 3, SQL_C_LONG, &nLevel, 0, &cbLevel);

                            std::wcout << L"\n===== DB 데이터 출력 조회 =====" << std::endl;

                            int nRowCount = 0;
                            // 5. 루프 돌면서 한 줄씩 데이터 가져오기
                            while (SQLFetch(hstmt) == SQL_SUCCESS) {
                                nRowCount++;

                                if (cbName == SQL_NULL_DATA) {
                                    wcscpy_s(wszName, L"N/A");
                                }

                                std::wcout << L"[" << nRowCount << L"] "
                                    << L"ID: " << nId
                                    << L" | 이름: " << wszName
                                    << L" | 레벨: " << nLevel << std::endl;
                            }
                        }
                        else
                        {
                            HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
                        }
                    }

                    SQLDisconnect(hdbc);
                }
                else
                {
                    HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
                }

                SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
            }
        }
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
    }
}