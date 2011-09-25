#pragma once

//#include "ExtString.h"
#include "EString.h"
#include "Exception.h"
#include "Callbacks.h"
#include "sqlite3.h"

static const wchar_t * SZ_SEPARATOR = L"|";

struct stRowSetMap // Introduced to prevent Warning C4503
{
public:
    multimap<CEString, vector<map<CEString, CEString>>::iterator> mmap;
};

class CSqlite
{
public:

    CSqlite()
    {
        ++m_iRefcount_;
    }

    CSqlite (const CEString& sDbPath) : m_bPreparedFromCache (false)
    {
        if (0 >= m_iRefcount_)
        {
            if (m_spDb_)
            {
                ATLASSERT(0);
                throw CException (-1, L"DB is initialized but ref count is 0.");
            }
            int iRet = sqlite3_open16 (sDbPath, &m_spDb_);
            if (SQLITE_OK != iRet)
            {
                m_spDb_ = NULL;
                throw CException (iRet, L"sqlite3_open16 failed.");
            }
        }
        else
        {
            if (!m_spDb_)
            {
                throw CException (-1, L"BD is not initialized but ref count is positive.");
            }
        }

        ++m_iRefcount_;
    }


    ~CSqlite()
    {
        --m_iRefcount_;
        if (0 == m_iRefcount_)
        {
            if (m_spDb_)
            {
                int iRet = sqlite3_close (m_spDb_);
                if (SQLITE_BUSY == iRet)
                {
                    ERROR_LOG (L"Warning: DB is still in use; trying to finalize open statements. \n");
                    for (int iCycle = 0; iCycle < 1000 && iRet == SQLITE_BUSY; ++iCycle)
                    {
                        sqlite3_stmt * stmt = sqlite3_next_stmt (m_spDb_, NULL);
                        iRet = sqlite3_finalize (stmt);
                        iRet = sqlite3_close (m_spDb_);
                    }
                    iRet = sqlite3_close (m_spDb_);
                    if (SQLITE_OK != iRet)
                    {
                        throw CException (iRet, L"Unable to close database");
                    }
                }
                m_spDb_ = NULL;
            }
        }
        
//        _CrtDumpMemoryLeaks();

    }   //  ~CSqlite()

    
private:
    static sqlite3 * m_spDb_;
    static int m_iRefcount_;

    sqlite3_stmt * m_pStmt;
    CEString m_sDbPath;

    map<CEString, vector<map<CEString, CEString>>> m_mapTables;
    map<CEString, int> m_mapIndices;                // table; parameter; link to the parameter map
    vector<map<CEString, int>> m_vecIdxParameters;  // parameter; link to the value-rows multimap
    vector<stRowSetMap> m_vecIdxRows;               // each multimap contains a value and
                                                    // a collection of pointers to corresponding rows
    vector<vector<CEString>> m_vecRowResult;
    bool m_bPreparedFromCache;                      // true iff the current SELECT result is stored in m_vecRowResult

    int m_iExtendedErrCode;

public:
    void Open (const CEString& sPath)
    {
        m_mapTables.clear();
        m_mapIndices.clear();
        m_vecIdxParameters.clear();
        m_vecIdxRows.clear();
        m_vecRowResult.clear();
        m_bPreparedFromCache = false;

        int iRet = sqlite3_open16 (sPath, &m_spDb_);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"Unable to open database");
        }
    }

    void BeginTransaction()
    {
        BeginTransaction (m_pStmt);
    }

    void BeginTransaction (unsigned int uiHandle)
    {
        BeginTransaction ((sqlite3_stmt *)uiHandle);
    }

    void BeginTransaction (sqlite3_stmt * pStmt)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        m_vecRowResult.clear();
        m_bPreparedFromCache = false;

        int iRet = SQLITE_OK;
        iRet = sqlite3_prepare16_v2 (m_spDb_, L"BEGIN;", -1, &pStmt, NULL);
        if (SQLITE_OK != iRet) 
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }

        iRet = sqlite3_step (pStmt);
        if (SQLITE_DONE != iRet) 
        {
            throw CException (iRet, L"sqlite3_step failed");
        }

        iRet = sqlite3_finalize (pStmt);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_finalize failed");
        }
    }

    void CommitTransaction()
    {
        CommitTransaction (m_pStmt);
    }

    void CommitTransaction (unsigned int uiHandle)
    {
        CommitTransaction ((sqlite3_stmt *)uiHandle);
    }

    void CommitTransaction (sqlite3_stmt * pStmt)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        m_vecRowResult.clear();

        int iRet = SQLITE_OK;
        iRet = sqlite3_prepare16_v2 (m_spDb_, L"COMMIT;", -1, &pStmt, NULL);
	    if (SQLITE_OK != iRet) 
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }

	    iRet = sqlite3_step (pStmt);
	    if (SQLITE_DONE != iRet) 
        {
            throw CException (iRet, L"sqlite3_step failed");
        }

	    iRet = sqlite3_finalize (pStmt);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_finalize failed");
        }
    
    }   //  CommitTransaction (...)

    void RollbackTransaction()
    {
        RollbackTransaction (m_pStmt);
    }

    void RollbackTransaction (unsigned int uiHandle)
    {
        RollbackTransaction ((sqlite3_stmt *)uiHandle);
    }

    void RollbackTransaction (sqlite3_stmt * pStmt)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        m_vecRowResult.clear();

        int iRet = SQLITE_OK;
        iRet = sqlite3_prepare16_v2 (m_spDb_, L"ROLLBACK;", -1, &pStmt, NULL);
        if (SQLITE_OK != iRet) 
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }

	    iRet = sqlite3_step (pStmt);
	    if (SQLITE_DONE != iRet) 
        {
            throw CException (iRet, L"sqlite3_step failed");
        }

	    iRet = sqlite3_finalize (pStmt);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_finalize failed");
        }
    
    }   //  RollbackTransaction (...)
    
    void Exec (const CEString& sStmt, void (*Callback)(sqlite3_stmt *, void *) = NULL, void * pArguments = NULL)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        int iRet = SQLITE_OK;
        sqlite3_stmt * pStmt = NULL;
        iRet = sqlite3_prepare16_v2 (m_spDb_, sStmt, -1, &pStmt, NULL);
        if (SQLITE_OK != iRet) 
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }

        iRet = sqlite3_step (pStmt);
        while (iRet == SQLITE_ROW) 
        {
            if (Callback)
            {
                (*Callback)(pStmt, pArguments);
            }
            iRet = sqlite3_step (pStmt);
        }
        if (SQLITE_ROW != iRet && SQLITE_DONE != iRet) 
        {
            throw CException (iRet, L"sqlite3_step failed");
        }

        iRet = sqlite3_finalize (pStmt);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_finalize failed");
        }

    }   // Exec()

    void PrepareForSelect (const CEString& sStmt)
    {
        PrepareForSelect (sStmt, m_pStmt);
    }

    unsigned int uiPrepareForSelect (const CEString& sStmt)
    {
        sqlite3_stmt * pStmt = NULL;
        PrepareForSelect (sStmt, pStmt);
        return (unsigned int)pStmt;
    }

    void PrepareForSelect (const CEString& sStmt, sqlite3_stmt *& pStmt)
    {
        // If the table has been loaded then search there; if not, search in the DB
        m_vecRowResult.clear();
        m_bPreparedFromCache = false;

        if (!m_mapTables.empty())
        {
            CEString sStmtRegex(sStmt);
            if (sStmtRegex.bRegexMatch(L"(select|SELECT|Select)\\s+(.*?)\\s+(from|FROM|From)\\s+([^\\s]+)\\s*(.*)"))
            {
                CEString sColumnsToSelect = sStmtRegex.sGetRegexMatch(1);
                CEString sTable = sStmtRegex.sGetRegexMatch(3);
                CEString sRest = sStmtRegex.sGetRegexMatch(4);

                vector<CEString> vecColumnsToSelect;

                sColumnsToSelect.SetBreakChars(L",");
                for (unsigned int uiField = 0; uiField < sColumnsToSelect.uiGetNumOfFields(); ++uiField)
                {
                    CEString sColumnName = sColumnsToSelect.sGetField(uiField);
                    sColumnName.TrimLeft();
                    vecColumnsToSelect.push_back(sColumnName);
                }

                if (m_mapTables.count(sTable) > 0 && vecColumnsToSelect.size() > 0)   // No conditions specified
                {
                    bool bOK = true;
                    if (sRest.uiLength() == 0)
                    {
                        if (vecColumnsToSelect[0] == L"*")
                        {
                            for (vector<map<CEString, CEString>>::iterator itRow = m_mapTables[sTable].begin();
                                 itRow != m_mapTables[sTable].end();
                                 ++itRow)
                            {
                                vector<CEString> vecRow;
                                for (map<CEString, CEString>::iterator itCol = (*itRow).begin();
                                     itCol != (*itRow).end();
                                     ++itCol)
                                {
                                    vecRow.push_back(itCol->second);
                                }
                            }
                        }
                        else
                        {
                            for (vector<map<CEString, CEString>>::iterator itRow = m_mapTables[sTable].begin();
                                 itRow != m_mapTables[sTable].end();
                                 ++itRow)
                            {
                                vector<CEString> vecRow;
                                for (vector<CEString>::iterator itCol = vecColumnsToSelect.begin();
                                     itCol != vecColumnsToSelect.end();
                                     ++itCol)
                                {
                                    if (itRow->count(*itCol) <= 0)
                                    {
                                        bOK = false;
                                        break;
                                    }
                                    vecRow.push_back((*itRow)[*itCol]);
                                }
                            }
                        }
                    }   // if (sRest.uiLength() == 0)...
                    else if (sRest.bRegexMatch(L"(WHERE|where|Where)\\s+([^\\s]+)\\s*=\\s*\\\"?([^\\s\\\"]+)\\\"?;?\\s*"))
                    {
                        // use the indices instead of browsing the table
                        CEString sParameter = sRest.sGetRegexMatch(1);
                        CEString sValue = sRest.sGetRegexMatch(2);

                        if (m_mapIndices.count(sTable) > 0 &&
                            m_vecIdxParameters[m_mapIndices[sTable]].count(sParameter) > 0)
                        {
                            stRowSetMap soRows = m_vecIdxRows[m_vecIdxParameters[m_mapIndices[sTable]][sParameter]];
                            pair<multimap<CEString, vector<map<CEString, CEString>>::iterator>::iterator,
                                 multimap<CEString, vector<map<CEString, CEString>>::iterator>::iterator> pairSearchResult;
                            pairSearchResult = soRows.mmap.equal_range(sValue);
                            if (vecColumnsToSelect[0] == L"*")
                            {
                                for (; pairSearchResult.first != pairSearchResult.second; ++pairSearchResult.first)
                                {
                                    vector<map<CEString, CEString>>::iterator itRow = pairSearchResult.first->second;
                                    vector<CEString> vecRow;
                                    for (map<CEString, CEString>::iterator itCol = (*itRow).begin();
                                         itCol != (*itRow).end();
                                         ++itCol)
                                    {
                                        vecRow.push_back(itCol->second);
                                    }
                                }
                            }
                            else
                            {
                                for (; pairSearchResult.first != pairSearchResult.second; ++pairSearchResult.first)
                                {
                                    vector<map<CEString, CEString>>::iterator itRow = pairSearchResult.first->second;
                                    vector<CEString> vecRow;
                                    for (vector<CEString>::iterator itCol = vecColumnsToSelect.begin();
                                         itCol != vecColumnsToSelect.end();
                                         ++itCol)
                                    {
                                        if (itRow->count(*itCol) <= 0)
                                        {
                                            bOK = false;
                                            break;
                                        }
                                        vecRow.push_back((*itRow)[*itCol]);
                                    }
                                }
                            }
                        }
                        else
                        {
                            bOK = false;
                        }
                    }   // else if (sRest.bRegexMatch(L"([^\\s]+)\\s*=\\s*([^\\s]+)\\s*"))...
                    else
                    {
                        bOK = false;
                    }

                    if (!bOK)
                    {
                        m_vecRowResult.clear();
                        m_bPreparedFromCache = false;
                    }
                    else
                    {
                        // Rows have been found and stored in m_vecRowResult; add one more row and return
                        vector<CEString> vecEmpty;
                        m_vecRowResult.push_back(vecEmpty);
                        m_bPreparedFromCache = true;
                        return;
                    }
                }
            }   // if (sStmtRegex.bRegexMatch...
        }

        int iRet = sqlite3_prepare16_v2 (m_spDb_, sStmt, -1, &pStmt, NULL);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }
    }

    void PrepareForInsert (const CEString& sTable, int iColumns)
    {
        uiPrepareForInsert (sTable, iColumns, m_pStmt);
    }

    unsigned int uiPrepareForInsert (const CEString& sTable, int iColumns, sqlite3_stmt *& pStmt)
    {
        m_vecRowResult.clear();
        m_bPreparedFromCache = false;
        vDropTable(sTable);

        CEString sStmt = L"INSERT INTO ";
        sStmt += sTable;
        sStmt += L" VALUES (NULL, ";
        for (int iCol = 0; iCol < iColumns; ++iCol)
        {
            if (iCol > 0)
            {
                sStmt += L",";
            }
            sStmt += L"?";
        }
        sStmt += L")";

        int iRet = sqlite3_prepare16_v2 (m_spDb_, sStmt, -1, &pStmt, NULL);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }

        return (unsigned int)pStmt;
    }
   
    void Bind (int iColumn, bool bValue)
    {
        Bind (iColumn, bValue, m_pStmt);
    }

    void Bind (int iColumn, bool bValue, unsigned int uiHandle)
    {
        Bind (iColumn, bValue, (sqlite3_stmt *)uiHandle);
    }

    void Bind (int iColumn, bool bValue, sqlite3_stmt * pStmt)
    {
        int iRet = sqlite3_bind_int (pStmt, iColumn, bValue);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_bind_int failed");
        }
    }

    void Bind (int iColumn, int iValue)
    {
        Bind (iColumn, iValue, m_pStmt);
    }

    void Bind (int iColumn, int iValue, unsigned int uiHandle)
    {
        Bind (iColumn, iValue, (sqlite3_stmt *)uiHandle);
    }

    void Bind (int iColumn, int iValue, sqlite3_stmt * pStmt)
    {
        int iRet = sqlite3_bind_int (pStmt, iColumn, iValue);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_bind_int failed");
        }
    }

    void Bind (int iColumn, unsigned int uiValue)
    {
        Bind (iColumn, uiValue, m_pStmt);
    }

    void Bind (int iColumn, unsigned int uiValue, unsigned int uiHandle)
    {
        Bind (iColumn, uiValue, (sqlite3_stmt *)uiHandle);
    }

    void Bind (int iColumn, unsigned int uiValue, sqlite3_stmt * pStmt)
    {
        int iRet = sqlite3_bind_int64 (pStmt, iColumn, (__int64)uiValue);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_bind_int64 failed");
        }
    }

    void Bind (int iColumn, __int64 llValue)
    {
        Bind (iColumn, llValue, m_pStmt);
    }

    void Bind (int iColumn, __int64 llValue, unsigned int uiHandle)
    {
        Bind (iColumn, llValue, (sqlite3_stmt *)uiHandle);
    }

    void Bind (int iColumn, __int64 llValue, sqlite3_stmt * pStmt)
    {
        int iRet = sqlite3_bind_int64 (pStmt, iColumn, llValue);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_bind_int64 failed");
        }
    }

    void Bind (int iColumn, const CEString& sValue)
    {
        Bind (iColumn, sValue, m_pStmt);
    }

    void Bind (int iColumn, const CEString& sValue, unsigned int uiHandle)
    {
        Bind (iColumn, sValue, (sqlite3_stmt *)uiHandle);
    }

    void Bind (int iColumn, const CEString& sValue, sqlite3_stmt * pStmt)
    {
        int iRet = sqlite3_bind_text16 (pStmt, iColumn, sValue, -1, SQLITE_STATIC);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_bind_text16 failed");
        }
    }

    void InsertRow()
    {
        InsertRow (m_pStmt);
    }

    void InsertRow (unsigned int uiHandle)
    {
        InsertRow ((sqlite3_stmt *)uiHandle);
    }

    void InsertRow (sqlite3_stmt * pStmt)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        if (NULL == pStmt)
        {
            throw CException (-1, L"No statement");
        }

        int iRet = sqlite3_step (pStmt);
        if (SQLITE_DONE != iRet)
        {
            throw CException (iRet, L"sqlite3_step failed");
        }

        iRet = sqlite3_reset (pStmt);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_reset failed");
        }

    }   // v_InsertRow()

    bool bGetRow()
    {
        return bGetRow (m_pStmt);
    }

    bool bGetRow (unsigned int uiHandle)
    {
        return bGetRow ((sqlite3_stmt *)uiHandle);
    }

    bool bGetRow (sqlite3_stmt * pStmt)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        if (NULL == pStmt)
        {
            throw CException (-1, L"No statement");
        }

        if (m_bPreparedFromCache)
        {
            m_vecRowResult.pop_back();
            if (m_vecRowResult.size() > 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        int iRet = sqlite3_step (pStmt);
        if (SQLITE_DONE == iRet)
        {
            iRet = sqlite3_reset (pStmt);
            return false;
        }

        if (SQLITE_ROW != iRet)
        {
            throw CException (iRet, L"sqlite3_step failed");
        }

        return true;

    }   // bGetRow (...)

    void GetData (int iColumn, bool& bValue)
    {
        GetData (iColumn, bValue, m_pStmt);
    }

    void GetData (int iColumn, bool& bValue, unsigned int uiHandle)
    {
        GetData (iColumn, bValue, (sqlite3_stmt *)uiHandle);
    }

    void GetData (int iColumn, bool& bValue, sqlite3_stmt * pStmt)
    {
        if (m_bPreparedFromCache)
        {
            wstring sRet = m_vecRowResult[m_vecRowResult.size() - 1][iColumn];
            bValue = (sRet != L"0");
        }
        else
        {
            int iRet = sqlite3_column_int (pStmt, iColumn);
            bValue = (iRet != 0);
        }
    }

    void GetData (int iColumn, int& iValue)
    {
        GetData (iColumn, iValue, m_pStmt);
    }

    void GetData (int iColumn, int& iValue, unsigned int uiHandle)
    {
        GetData (iColumn, iValue, (sqlite3_stmt *)uiHandle);
    }

    void GetData (int iColumn, int& iValue, sqlite3_stmt * pStmt)
    {
        if (m_bPreparedFromCache)
        {
            wstringstream ws;
            ws << m_vecRowResult[m_vecRowResult.size() - 1][iColumn];
            ws >> iValue;
        }
        else
        {
            iValue = sqlite3_column_int (pStmt, iColumn);
        }
    }

    void GetData (int iColumn, __int64& ll_value)
    {
        GetData (iColumn, ll_value, m_pStmt);
    }

    void GetData (int iColumn, __int64& ll_value, unsigned int uiHandle)
    {
        GetData (iColumn, ll_value, (sqlite3_stmt *)uiHandle);
    }

    void GetData (int iColumn, __int64& ll_value, sqlite3_stmt * pStmt)
    {
        if (m_bPreparedFromCache)
        {
            wstringstream ws;
            ws << m_vecRowResult[m_vecRowResult.size() - 1][iColumn];
            ws >> ll_value;
        }
        else
        {
            ll_value = sqlite3_column_int64 (pStmt, iColumn);
        }
    }

    void GetData (int iColumn, CEString& sValue)
    {
        GetData (iColumn, sValue, m_pStmt);
    }

    void GetData (int iColumn, CEString& sValue, unsigned int uiHandle)
    {
        GetData (iColumn, sValue, (sqlite3_stmt *)uiHandle);
    }

    void GetData (int iColumn, CEString& sValue, sqlite3_stmt * pStmt)
    {
        if (m_bPreparedFromCache)
        {
            sValue = m_vecRowResult[m_vecRowResult.size() - 1][iColumn];
        }
        else
        {
            const void * p_ = sqlite3_column_text16 (pStmt, iColumn);
            if (p_)
            {
                sValue = static_cast<wchar_t *>(const_cast<void *>(p_));
            }
        }
    }

    void Finalize()
    {
        Finalize (m_pStmt);
    }

    void Finalize (unsigned int uiHandle)
    {
        Finalize ((sqlite3_stmt *)uiHandle);
    }

    void Finalize (sqlite3_stmt * pStmt)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        if (NULL == pStmt)
        {
            throw CException (-1, L"No statement handle");
        }

        m_vecRowResult.clear();
        m_bPreparedFromCache = false;

        int iRet = sqlite3_finalize (pStmt);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_finalize failed");
        }
    }

    __int64 llGetLastKey()
    {
        return llGetLastKey (m_pStmt);
    }

    __int64 llGetLastKey (unsigned int uiHandle)
    {
        return llGetLastKey ((sqlite3_stmt *)uiHandle);
    }

    __int64 llGetLastKey (sqlite3_stmt * pStmt)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        if (NULL == pStmt)
        {
            throw CException (-1, L"No statement handle");
        }

        return sqlite3_last_insert_rowid (m_spDb_);    
    }

    int iGetLastError()
    {
        if (NULL == m_spDb_)
        {
            return -1;
        }

        return sqlite3_extended_errcode (m_spDb_);
    }

    void GetLastError (CEString& sError)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        wchar_t * szError = (wchar_t *)sqlite3_errmsg16 (m_spDb_);
        sError = szError;
    }

    bool bTableExists (const CEString& sTable)
    {
        CEString sQuery (L"SELECT name FROM sqlite_master WHERE type='table';");
        int iRet = sqlite3_prepare16_v2 (m_spDb_, sQuery, -1, &m_pStmt, NULL);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }

        do
        {
            iRet = sqlite3_step (m_pStmt);
            if (SQLITE_DONE == iRet)
            {
                iRet = sqlite3_reset (m_pStmt);
                Finalize();
                return false;
            }

            if (SQLITE_ROW != iRet)
            {
                throw CException (iRet, L"sqlite3_step failed");
            }
 
            CEString sCurrent;
            GetData (0, sCurrent);
            if (sTable == sCurrent)
            {
                Finalize();
                return true;
            }

        } while (SQLITE_ROW == iRet);

        Finalize();
        return false;

    }   //  b_TableExists (...)

    bool bTableEmpty (const CEString& sTable)
    {
        CEString sQuery (L"SELECT * FROM ");
        sQuery += sTable;
        sQuery += L";";
        int iRet = sqlite3_prepare16_v2 (m_spDb_, sQuery, -1, &m_pStmt, NULL);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }

        iRet = sqlite3_step (m_pStmt);
        if (SQLITE_DONE == iRet)
        {
            iRet = sqlite3_reset (m_pStmt);
            return false;
        }

        if (SQLITE_ROW != iRet)
        {
            throw CException (iRet, L"sqlite3_step failed");
        }

        return true;

    }   //  TableEmpty (...)

    __int64 llRows (const CEString& sTable)
    {
        CEString sQuery (L"SELECT COUNT (*) FROM ");
        sQuery += sTable;
        sQuery += L";";
        int iRet = sqlite3_prepare16_v2 (m_spDb_, sQuery, -1, &m_pStmt, NULL);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed");
        }

        iRet = sqlite3_step (m_pStmt);
        if (SQLITE_DONE == iRet)
        {
            iRet = sqlite3_reset (m_pStmt);
            return 0;
        }

        if (SQLITE_ROW != iRet)
        {
            throw CException (iRet, L"sqlite3_step failed");
        }

        return sqlite3_column_int64 (m_pStmt, 0);

    }   //  llRows (...)

    bool bExportTables (const CEString& sPath,
                        const vector<CEString>& vecTables,
                        CProgressCallback& Progress)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        FILE * ioOutStream = NULL;
        errno_t iError = _tfopen_s (&ioOutStream, sPath, L"w, ccs=UNICODE");
        if (0 != iError)
        {
            CString csMsg;
            csMsg.Format (L"Unable to open export file, error %d", iError);
            throw CException (-1, (LPCTSTR)csMsg);
        }

        vector<CEString>::const_iterator itTable  = vecTables.begin();

        __int64 llRowsToExport = 0;
        for (itTable = vecTables.begin(); 
             itTable != vecTables.end();
             ++itTable)
        {
            llRowsToExport += llRows (*itTable);
        }
        
        if (llRowsToExport < 1)
        {
            return true;
        }

        __int64 llRow = 0;
        
        for (vector<CEString>::const_iterator itTable = vecTables.begin(); 
             itTable != vecTables.end();
             ++itTable)
        {
            CEString sQuery (L"SELECT * FROM ");
//            str_query += sTable;
            sQuery += *itTable;
            sQuery += L";";

            sqlite3_stmt * pStmt = NULL;
            int iRet = sqlite3_prepare16_v2 (m_spDb_, sQuery, -1, &pStmt, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"sqlite3_prepare16_v2 failed");
            }

            CEString sTableName (*itTable);
            sTableName += L"\n";
            iError = _fputts (sTableName, ioOutStream);
            if (0 != iError)
            {
                ERROR_LOG (L"Error writing export table name. \n");
            }

            CEString sHeader;
            int iColumns = sqlite3_column_count (pStmt);
            for (int iColName = 0; iColName < iColumns; ++iColName)
            {
                sHeader += (wchar_t *)sqlite3_column_name16 (pStmt, iColName);
                if (iColName < iColumns - 1)
                {
                    sHeader += SZ_SEPARATOR;
                }
            }
            sHeader += L"\n";

            iError = _fputts (sHeader, ioOutStream);
            if (0 != iError)
            {
                ERROR_LOG (L"Error writing export table header. \n");
            }

            int iPercentDone = 0;
            while (bGetRow (pStmt))
            {
                CEString sOut;
                for (int iCol = 0; iCol < iColumns; ++iCol)
                {
                    CEString sCol;
                    GetData (iCol, sCol, pStmt);
                    if (sOut.uiLength() > 0)
                    {
                        sOut += SZ_SEPARATOR;
                    }
                    sOut += sCol;
                }
                sOut += L"\n";

                iError = _fputts (sOut, ioOutStream);
                if (0 != iError)
                {
                    throw CException (iError, L"Error writing export table header.");
                }
                
                int iPd = (int) (((double)llRow/(double)llRowsToExport) * 100);
                if (iPd > iPercentDone)
                {
                    iPercentDone = min (iPd, 100);
                    Progress (iPercentDone);
                }

                ++llRow;

            }       //  while (...)
            
            iError = _fputts (L"\n", ioOutStream);
            if (0 != iError)
            {
                ERROR_LOG (L"Error writing terminating line. \n");
            }

        }   //  for (vector<CEString> ...

        fclose (ioOutStream);

        return true;

    }   //  ExportTables (...)

    //
    // Note: existing tables will be overwritten
    //
    bool bImportTables (const CEString& sPath, 
                        CProgressCallback& Progress)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        m_mapTables.clear();
        m_mapIndices.clear();
        m_vecIdxParameters.clear();
        m_vecIdxRows.clear();
        m_vecRowResult.clear();
        m_bPreparedFromCache = false;

        FILE * ioInStream = NULL;
        errno_t iError = _tfopen_s (&ioInStream, sPath, L"r, ccs=UNICODE");
        if (0 != iError)
        {
            CString csMsg;
            csMsg.Format (L"Unable to open import file, error %d", iError);
            throw CException (-1, csMsg);
        }

        int iCharsRead = 0;
        int iPercentDone = 0;
//        int iEntriesRead = 0;

        TCHAR szLineBuf[10000];

        while (!feof (ioInStream))
        {
            //
            // Get table name
            //
            CEString sTable;
            while (!feof (ioInStream) && sTable.bIsEmpty())
            {
                TCHAR * szRet = _fgetts (szLineBuf, 10000, ioInStream);
                if (NULL == szRet)
                {
                    iError = ferror (ioInStream);
                    if (0 != iError)
                    {
                        throw CException (iError, L"Error reading table name.");
                    }
                }
                else
                {
                    sTable = szLineBuf;
                }
            }

            if (feof (ioInStream))
            {
                continue;
            }

            if (sTable.bIsEmpty())
            {
                ATLASSERT(0);
                throw CException (-1, L"Empty table name.");
            }

            sTable.Trim (L"\n ");

            //
            // Get table descriptor
            //
            CEString sDescriptor;
            while (!feof (ioInStream) && sDescriptor.bIsEmpty())
            {
                TCHAR * szRet = _fgetts (szLineBuf, 10000, ioInStream);
                if (NULL == szRet)
                {
                    iError = ferror (ioInStream);
                    if (0 != iError)
                    {
                        throw CException (iError, L"Error reading import file header.");
                    }
                }
                else
                {
                    sDescriptor = szLineBuf;
                }
            }

            sDescriptor.Trim (L"\n ");

            if (feof (ioInStream))
            {
                continue;
            }

            if (sDescriptor.bIsEmpty())
            {
                ATLASSERT(0);
                throw CException (-1, L"Empty table descriptor.");
            }

            int iColumns = 0;
            bool bRet = bCreateImportTable (sTable, sDescriptor, iColumns);
            if (!bRet)
            {
                throw CException (-1, L"Unable to create import table.");
            }

            bRet = bImport (ioInStream, sTable, iColumns, iCharsRead, Progress);
            if (!bRet)
            {
                throw CException (-1, L"Table import failed.");
            }

        }   //  while (!feof (ioInstream))
    
        Progress (100);
        fclose (ioInStream);
        
        return true;
        
    }   //  ImportTables (...)

    //
    //  Helpers
    //
    bool bCreateImportTable (const CEString& sTable, const CEString& sDescriptor, int& iColumns)
    {
        CEString sSeparators (SZ_SEPARATOR);
        sSeparators += L", \n";
        CEString sHeader (sDescriptor);
        sHeader.SetBreakChars (SZ_SEPARATOR);
        if (sHeader.uiGetNumOfFields() < 1)
        {
            throw CException (-1, L"Parsing error: no fields.");
        }

        iColumns = sHeader.uiNFields();

        if (bTableExists (sTable))
        {
            CEString sDropStmt (L"DROP TABLE ");
            sDropStmt += sTable;

            sqlite3_stmt * pStmt = NULL;
            int iRet = sqlite3_prepare16_v2 (m_spDb_, sDropStmt, -1, &pStmt, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"sqlite3_prepare16_v2 failed for drop.");
            }

	        iRet = sqlite3_step (pStmt);
	        if (SQLITE_DONE != iRet) 
            {
                throw CException (iRet, L"sqlite3_step failed for drop.");
            }

            sqlite3_reset (pStmt);
        }
        
        CEString sCreateStmt (L"CREATE TABLE ");
        sCreateStmt += sTable;
        sCreateStmt += L" (";
        sCreateStmt += sHeader.sGetField (0);
        sCreateStmt += L" INTEGER PRIMARY KEY ASC";
        for (int iCol = 1; iCol < iColumns; ++iCol)
        {
            sCreateStmt += L", ";
            sCreateStmt += sHeader.sGetField (iCol);
            sCreateStmt += L" TEXT";
        }
        sCreateStmt += L");";

        sqlite3_stmt * pStmt = NULL;
        int iRet = sqlite3_prepare16_v2 (m_spDb_, sCreateStmt, -1, &pStmt, NULL);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_prepare16_v2 failed for create.");
        }

	    iRet = sqlite3_step (pStmt);
	    if (SQLITE_DONE != iRet) 
        {
            throw CException (iRet, L"sqlite3_step failed for create.");
        }

        iRet = sqlite3_finalize (pStmt);
        if (SQLITE_OK != iRet)
        {
            throw CException (iRet, L"sqlite3_finalize failed for create.");
        }

        return true;
    
    }   //  b_CreateImportTable (...)

    bool bImport (FILE * ioInstream, 
                  const CEString& sTable, 
                  int iColumns,
                  int iCharsRead,
                  CProgressCallback& Progress)
    {
        long lFileLength = _filelength (_fileno (ioInstream))/sizeof (wchar_t);
        int iPercentDone = 0;

        m_mapTables.clear();
        m_mapIndices.clear();
        m_vecIdxParameters.clear();
        m_vecIdxRows.clear();
        m_vecRowResult.clear();
        m_bPreparedFromCache = false;

        CEString sStmt = L"INSERT INTO ";
        sStmt += sTable;
        sStmt += L" VALUES (";
        for (int iCol = 0; iCol < iColumns; ++iCol)
        {
            if (iCol > 0)
            {
                sStmt += L",";
            }
            sStmt += L"?";
        }
        sStmt += L")";

        sqlite3_stmt * pStmt = NULL;

        BeginTransaction (pStmt);
       
        CEString sSeparators (SZ_SEPARATOR);
        sSeparators += L", \n";
        int iEntriesRead = 0;

        TCHAR szLineBuf[10000];
        CEString sLine;
        sLine.SetBreakChars (sSeparators);
        for (; !feof (ioInstream); ++iEntriesRead)
        {
            int iRet = sqlite3_prepare16_v2 (m_spDb_, sStmt, -1, &pStmt, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"sqlite3_prepare16_v2 failed");
            }

            TCHAR * szRet = _fgetts (szLineBuf, 10000, ioInstream);
            sLine = szLineBuf;
            sLine.Trim (sSeparators);
            if (NULL == szRet)
            {
                errno_t iError = ferror (ioInstream);
                if (0 != iError)
                {
                    throw CException (iRet, L"Error reading import file.");
                }
                else
                {
                    continue;
                }
            }

            if (sLine.bIsEmpty())
            {
                break;
            }

            iCharsRead += sLine.uiLength();

            if (sLine.uiNFields() != iColumns)
            {
                throw CException (-1, L"Number of fields does not match number of columns.");
            }

            __int64 llId = _wtoi64 (sLine.sGetField (0));
            Bind (1, llId, pStmt);

            for (int iCol = 2; iCol <= iColumns; ++iCol)
            {
                Bind (iCol, sLine.sGetField (iCol-1), pStmt);
            }
            
            InsertRow (pStmt);
            Finalize (pStmt);

            int iPd = (int) (((double)iCharsRead/(double)lFileLength) * 100);
            if (iPd > iPercentDone)
            {
                iPercentDone = min (iPd, 100);
                Progress (iPercentDone);
            }

        }   //  for (; !feof (ioInstream); ++iEntriesRead)

        CommitTransaction (pStmt);

        return true;

    }   // Import (...)

    int iLastID (const CEString& sTableName)     // returns the ID of the last entry in the table
    {
        int iLastId = 0;
        CEString sQuery = L"Select * from " + sTableName 
            + L" as a0 where not exists (select * from " + sTableName 
            + L" as a1 where a1.id > a0.id)";
        PrepareForSelect (sQuery);
        if (bGetRow())
        {
            GetData(0, iLastId);
        }
        else
        {
            iLastId = -1;
        }
        
        Finalize();
        
        return iLastId;
    
    }   // i_LastID (...)


    //
    //  Load DB to the memory
    //

public:
    bool bLoadTables(const vector<CEString>& vecTables) //, CProgressCallback& Progress)
    {
        if (NULL == m_spDb_)
        {
            throw CException (-1, L"No DB handle");
        }

        m_mapTables.clear();
        m_mapIndices.clear();
        m_vecIdxParameters.clear();
        m_vecIdxRows.clear();
        m_vecRowResult.clear();
        m_bPreparedFromCache = false;
        vector<CEString>::const_iterator itTable = vecTables.begin();

        __int64 llRowsToExport = 0;
        for (itTable = vecTables.begin(); 
             itTable != vecTables.end();
             ++itTable)
        {
            llRowsToExport += llRows(*itTable);
        }
        
        if (llRowsToExport < 1)
        {
            return true;
        }

        __int64 llRow = 0;
        
        for (vector<CEString>::const_iterator itTable = vecTables.begin(); 
             itTable != vecTables.end();
             ++itTable)
        {
            vector<CEString> vecColNames;
            vector<map<CEString, CEString>> vecRows;
            CEString sQuery(L"SELECT * FROM ");
            sQuery += *itTable;
            sQuery += L";";

            sqlite3_stmt * pStmt = NULL;
            int iRet = sqlite3_prepare16_v2(m_spDb_, sQuery, -1, &pStmt, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException(iRet, L"sqlite3_prepare16_v2 failed");
            }

            int iColumns = sqlite3_column_count(pStmt);
            for (int iColName = 0; iColName < iColumns; ++iColName)
            {
                vecColNames.push_back((wchar_t *)sqlite3_column_name16(pStmt, iColName));
            }

            int iPercentDone = 0;
            while (bGetRow(pStmt))
            {
                CEString sOut;
                map<CEString, CEString> mapRow;
                for (int iCol = 0; iCol < iColumns; ++iCol)
                {
                    CEString sCol;
                    GetData(iCol, sCol, pStmt);
                    mapRow.insert(pair<CEString, CEString>(vecColNames[iCol], sCol));
                }
                vecRows.push_back(mapRow);
                
                /*
                int iPd = (int)(((double)llRow / (double)llRowsToExport) * 100);
                if (iPd > iPercentDone)
                {
                    iPercentDone = min(iPd, 100);
                    Progress(iPercentDone);
                }
                */

                ++llRow;
            }       //  while (...)
            
            m_mapTables.insert(pair<CEString, vector<map<CEString, CEString>>>(*itTable, vecRows));
        }   //  for (vector<CEString> ...
        return true;
    }

    void vDropTable(CEString sTable)
    {
        m_bPreparedFromCache = false;
        m_vecRowResult.clear();
        if (!m_mapTables.empty())
        {
            if (m_mapTables.count(sTable) > 0)
            {
                m_mapTables.erase(sTable);
            }
        }
        if (!m_mapIndices.empty())
        {
            if (m_mapIndices.count(sTable) > 0)
            {
                for (map<CEString, int>::iterator itParameter = m_vecIdxParameters[m_mapIndices[sTable]].begin();
                     itParameter != m_vecIdxParameters[m_mapIndices[sTable]].end();
                     ++itParameter)
                {
                    m_vecIdxRows[itParameter->second].mmap.clear();
                }
                m_vecIdxParameters[m_mapIndices[sTable]].clear();
                m_mapIndices.erase(sTable);
            }
        }
    }

    bool bCreateIndex(CEString sTable, CEString sCol)
    {
        m_bPreparedFromCache = false;
        m_vecRowResult.clear();

        if (m_mapTables.count(sTable) <= 0)
        {
            return false;
        }
        
        stRowSetMap soIndex;
        map<CEString, vector<map<CEString, CEString>>>::iterator itTable = m_mapTables.find(sTable);
        for (vector<map<CEString, CEString>>::iterator itRows = itTable->second.begin();
             itRows != itTable->second.end();
             ++itRows)
        {
            if (itRows->count(sCol) <= 0)
            {
                return false;
            }
            soIndex.mmap.insert(pair<CEString, vector<map<CEString, CEString>>::iterator>((*itRows)[sCol], itRows));
        }
        m_vecIdxRows.push_back(soIndex);
        if (m_mapIndices.count(sTable) > 0)
        {
            if (m_vecIdxParameters[m_mapIndices[sTable]].count(sCol) > 0)
            {
                m_vecIdxRows[m_vecIdxParameters[m_mapIndices[sTable]][sCol]].mmap.clear();
                m_vecIdxParameters[m_mapIndices[sTable]][sCol] = m_vecIdxRows.size() - 1;
            }
            else
            {
                m_vecIdxParameters[m_mapIndices[sTable]].insert(pair<CEString, int>(sCol, m_vecIdxRows.size() - 1));
            }
        }
        else
        {
            map<CEString, int> mapNewTableParameters;
            mapNewTableParameters.insert(pair<CEString, int>(sCol, m_vecIdxRows.size() - 1));
            m_vecIdxParameters.push_back(mapNewTableParameters);
            m_mapIndices.insert(pair<CEString, int>(sTable, m_vecIdxParameters.size() - 1));
        }
        return true;
    }
};  //  class CSqlite

