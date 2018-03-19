#ifndef H_SQLITE_WRAPPER
#define H_SQLITE_WRAPPER

#include <io.h>
#include "EString.h"
#include "Exception.h"
#include "Callbacks.h"
#include "sqlite3.h"

static const wchar_t * SZ_SEPARATOR = L"|";

namespace Hlib
{
    // Progress delegate invoked from C#/CLR
    typedef void(__stdcall *PROGRESS_CALLBACK_CLR) (int iPercentDone, bool bOperationComplete);

    class CSqlite
    {
    private:

    static sqlite3 * m_spDb_;
    static int m_iRefcount_;

    public:

        CSqlite()
        {
            ++m_iRefcount_;
        }

        CSqlite (const CEString& sDbPath)
        {
            if (0 >= m_iRefcount_)
            {
                if (m_spDb_)
                {
                    ASSERT(0);
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
                        for (int iCycle = 0; iCycle < 10000 && iRet == SQLITE_BUSY; ++iCycle)
                        {
                            sqlite3_stmt * stmt = sqlite3_next_stmt (m_spDb_, NULL);
                            iRet = sqlite3_finalize (stmt);
                            iRet = sqlite3_close (m_spDb_);
                        }
//                        iRet = sqlite3_close (m_spDb_);
                        if (SQLITE_OK != iRet)
                        {
                            ERROR_LOG(L"Unable to close database.\n");
                        }
                    }
                    m_spDb_ = NULL;
                }
            }
        
    //        _CrtDumpMemoryLeaks();

        }   //  ~CSqlite()

    
    private:

        sqlite3_stmt * m_pStmt;
        CEString m_sDbPath;

        int m_iExtendedErrCode;

    public:
        void Open (const CEString& sPath)
        {
            int iRet = sqlite3_open16 (sPath, &m_spDb_);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"Unable to open database");
            }
        }

        void BeginTransaction ()
        {
            if (NULL == m_spDb_)
            {
                throw CException (-1, L"No DB handle");
            }

            int iRet = SQLITE_OK;
            iRet = sqlite3_exec(m_spDb_, "BEGIN TRANSACTION", NULL, NULL, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"sqlite3_exec failed for transaction start");
            }
        }

        void CommitTransaction()
        {
            if (NULL == m_spDb_)
            {
                throw CException (-1, L"No DB handle");
            }

            int iRet = SQLITE_OK;
            iRet = sqlite3_exec(m_spDb_, "END TRANSACTION", NULL, NULL, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"sqlite3_exec failed for transaction end");
            }
    
        }

        void RollbackTransaction()
        {
            if (NULL == m_spDb_)
            {
                throw CException(-1, L"No DB handle");
            }

            int iRet = SQLITE_OK;
            iRet = sqlite3_exec(m_spDb_, "END TRANSACTION;", NULL, NULL, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException(iRet, L"sqlite3_prepare16_v2 failed");
            }
        }

        void PrepareForSelect (const CEString& sStmt, bool bIgnoreOnConflict = false)
        {
            PrepareForSelect (sStmt, m_pStmt);
        }

        uint64_t uiPrepareForSelect(const CEString& sStmt, bool bIgnoreOnConflict = false)
        {
            sqlite3_stmt * pStmt = NULL;
            PrepareForSelect (sStmt, pStmt);
            return (uint64_t)pStmt;
        }

        void PrepareForSelect(const CEString& sStmt, sqlite3_stmt *& pStmt)
        {
            int iRet = sqlite3_prepare16_v2 (m_spDb_, sStmt, -1, &pStmt, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"sqlite3_prepare16_v2 failed");
            }
        }

        void PrepareForInsert (const CEString& sTable, int iColumns, bool bIgnoreOnConflict = false)
        {
            uiPrepareForInsert (sTable, iColumns, m_pStmt, bIgnoreOnConflict);
        }

        uint64_t uiPrepareForInsert (const CEString& sTable, int iColumns, sqlite3_stmt *& pStmt, bool bIgnoreOnConflict = false)
        {
            CEString sStmt(L"INSERT ");
            if (bIgnoreOnConflict)
            {
                sStmt += L"OR IGNORE ";
            }

            sStmt += L"INTO ";
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
                CEString sErrTxt;
                GetLastError(sErrTxt);
                CEString sMsg(L"sqlite3_prepare16_v2 failed: ");
                sMsg += sErrTxt;
                throw CException (iRet, sMsg);
            }

            return (uint64_t)pStmt;
        }

        void PrepareForUpdate (const CEString& sTable, const vector<CEString>& vecColumns, __int64 llPrimaryKey)
        {
            uiPrepareForUpdate (sTable, vecColumns, llPrimaryKey, m_pStmt);
        }

        void PrepareForUpdate (const CEString& sTable, const vector<CEString>& vecColumns)
        {
            uiPrepareForUpdate (sTable, vecColumns, -1, m_pStmt);
        }

        uint64_t uiPrepareForUpdate (const CEString& sTable, const vector<CEString>& vecColumns, __int64 llPrimaryKey, sqlite3_stmt *& pStmt)
        {
            CEString sStmt = L"UPDATE ";
            sStmt += sTable;
            sStmt += L" SET ";
            for (int iCol = 0; iCol < (int)vecColumns.size(); ++iCol)
            {
                if (iCol > 0)
                {
                    sStmt += L", ";
                }
                sStmt += vecColumns[iCol];
                sStmt += L"=";
                sStmt += L"?";
            }

            if (llPrimaryKey > -1)
            {
                sStmt += L" WHERE ID = ";
                sStmt += CEString::sToString(llPrimaryKey);
            }

            int iRet = sqlite3_prepare16_v2 (m_spDb_, sStmt, -1, &pStmt, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"sqlite3_prepare16_v2 failed");
            }

            return (uint64_t)pStmt;
        }

        void Bind (int iColumn, bool bValue)
        {
            Bind (iColumn, bValue, m_pStmt);
        }

        void Bind (int iColumn, bool bValue, uint64_t uiHandle)
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

        void Bind (int iColumn, int iValue, uint64_t uiHandle)
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

        void Bind (int iColumn, uint64_t uiValue)
        {
            Bind (iColumn, uiValue, m_pStmt);
        }

        void Bind (int iColumn, uint64_t uiValue, uint64_t uiHandle)
        {
            Bind (iColumn, uiValue, (sqlite3_stmt *)uiHandle);
        }

        void Bind (int iColumn, uint64_t uiValue, sqlite3_stmt * pStmt)
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

        void Bind (int iColumn, __int64 llValue, uint64_t uiHandle)
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

        void Bind (int iColumn, const CEString& sValue, uint64_t uiHandle)
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

        void InsertRow (uint64_t uiHandle)
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

        }   // InsertRow()

        void UpdateRow()
        {
            UpdateRow (m_pStmt);
        }

        void UpdateRow (uint64_t uiHandle)
        {
            UpdateRow ((sqlite3_stmt *)uiHandle);
        }

        void UpdateRow (sqlite3_stmt * pStmt)
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

        }   // InsertRow()

        bool bGetRow()
        {
            return bGetRow (m_pStmt);
        }

        bool bGetRow (uint64_t uiHandle)
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

        void GetData (int iColumn, bool& bValue, uint64_t uiHandle)
        {
            GetData (iColumn, bValue, (sqlite3_stmt *)uiHandle);
        }

        void GetData (int iColumn, bool& bValue, sqlite3_stmt * pStmt)
        {
            int iRet = sqlite3_column_int (pStmt, iColumn);
            bValue = (iRet != 0);
        }

        void GetData (int iColumn, int& iValue)
        {
            GetData (iColumn, iValue, m_pStmt);
        }

        void GetData (int iColumn, int& iValue, uint64_t uiHandle)
        {
            GetData (iColumn, iValue, (sqlite3_stmt *)uiHandle);
        }

        void GetData (int iColumn, int& iValue, sqlite3_stmt * pStmt)
        {
            iValue = sqlite3_column_int (pStmt, iColumn);
        }

        void GetData (int iColumn, __int64& ll_value)
        {
            GetData (iColumn, ll_value, m_pStmt);
        }

        void GetData (int iColumn, __int64& llValue, uint64_t uiHandle)
        {
            GetData (iColumn, llValue, (sqlite3_stmt *)uiHandle);
        }

        void GetData (int iColumn, __int64& llValue, sqlite3_stmt * pStmt)
        {
            llValue = (unsigned __int64)sqlite3_column_int64 (pStmt, iColumn);
        }

        void GetData(int iColumn, uint64_t& uiValue)
        {
            GetData(iColumn, uiValue, m_pStmt);
        }

        void GetData(int iColumn, unsigned int& uiValue, uint64_t uiHandle)
        {
            GetData(iColumn, uiValue, (sqlite3_stmt *)uiHandle);
        }

        void GetData(int iColumn, unsigned int& uiValue, sqlite3_stmt * pStmt)
        {
            uiValue = sqlite3_column_int(pStmt, iColumn);
        }

//        void GetData(int iColumn, unsigned __int64& ull_value)
//        {
//            GetData(iColumn, ull_value, m_pStmt);
//        }

        void GetData(int iColumn, unsigned __int64& ull_value, uint64_t uiHandle)
        {
            GetData(iColumn, ull_value, (sqlite3_stmt *)uiHandle);
        }

        void GetData(int iColumn, unsigned __int64& ull_value, sqlite3_stmt * pStmt)
        {
            ull_value = sqlite3_column_int64(pStmt, iColumn);
        }

        void GetData(int iColumn, CEString& sValue)
        {
            GetData (iColumn, sValue, m_pStmt);
        }

        void GetData (int iColumn, CEString& sValue, uint64_t uiHandle)
        {
            GetData (iColumn, sValue, (sqlite3_stmt *)uiHandle);
        }

        void GetData (int iColumn, CEString& sValue, sqlite3_stmt * pStmt)
        {
            const void * p_ = sqlite3_column_text16 (pStmt, iColumn);
            if (p_)
            {
                sValue = static_cast<wchar_t *>(const_cast<void *>(p_));
            }
        }

        void Finalize()
        {
            Finalize (m_pStmt);
        }

        void Finalize (uint64_t uiHandle)
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

            int iRet = sqlite3_finalize (pStmt);
            if (SQLITE_OK != iRet)
            {
                throw CException (iRet, L"sqlite3_finalize failed");
            }
        }

        void Exec(const CEString& sQuery)
        {
            size_t charsConverted = 0;
            int iMaxUtf8SizeInBytes = 2 * sQuery.uiLength() + 1;

            char * pchrUtf8Query = new char[iMaxUtf8SizeInBytes];
            if (NULL == pchrUtf8Query)
            {
                throw CException(H_ERROR_POINTER, L"Unable to allocate memory.");
            }

            errno_t errorCode = wcstombs_s(&charsConverted, pchrUtf8Query, iMaxUtf8SizeInBytes, sQuery, sQuery.uiLength());
            if (errorCode != 0)
            {
                throw CException(H_ERROR_POINTER, L"UTF-16 to URF-8 conversion error or bad query string.");
            }

            int iRet = sqlite3_exec(m_spDb_, pchrUtf8Query, NULL, NULL, NULL);
            if (SQLITE_OK != iRet)
            {
                throw CException(iRet, L"sqlite3_finalize failed");
            }

            delete[] pchrUtf8Query;
        }

        __int64 llGetLastKey()
        {
            return llGetLastKey (m_pStmt);
        }

        __int64 llGetLastKey (uint64_t uiHandle)
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

        bool bExportTables(const CEString& sPath, const vector<CEString>& vecTables, PROGRESS_CALLBACK_CLR pProgress)
        {
            if (NULL == m_spDb_)
            {
                throw CException (-1, L"No DB handle");
            }

            FILE * ioOutStream = NULL;
            errno_t iError = _tfopen_s (&ioOutStream, sPath, L"w, ccs=UNICODE");
            if (0 != iError)
            {
                CEString sMsg(L"Unable to open export file, error ");
                sMsg += CEString::sToString(iError);
                throw CException (-1, sMsg);
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
                        pProgress (iPercentDone, false);
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

            pProgress(100, false);

            return true;

        }   //  ExportTables (...)

        //
        // Note: existing tables will be overwritten
        //
        bool bImportTables(const CEString& sPath, bool bMerge, PROGRESS_CALLBACK_CLR pProgress)
        {
            if (NULL == m_spDb_)
            {
                throw CException (-1, L"No DB handle");
            }

            FILE * ioInStream = NULL;
            errno_t iError = _tfopen_s (&ioInStream, sPath, L"r, ccs=UNICODE");
            if (0 != iError)
            {
                CEString sMsg(L"Unable to open import file, error ");
                    sMsg += CEString::sToString(iError);
                throw CException (-1, sMsg);
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
                    ASSERT(0);
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
                    ASSERT(0);
                    throw CException (-1, L"Empty table descriptor.");
                }

                CEString sSeparators(SZ_SEPARATOR);
                sSeparators += L", \n";
                CEString sHeader(sDescriptor);
                sHeader.ResetSeparators();
                sHeader.SetBreakChars(sSeparators);
                if (sHeader.uiGetNumOfFields() < 1)
                {
                    throw CException(-1, L"Parsing error: no fields.");
                }

                int iColumns = sHeader.uiNFields();

                if (!bMerge)
                {
                    bool b_ = bCreateImportTable(sTable, sDescriptor, iColumns);
                    if (!b_)
                    {
                        throw CException(-1, L"Unable to create import table.");
                    }
                }

                bool bRet = bImport (ioInStream, sTable, iColumns, iCharsRead, bMerge, pProgress);
                if (!bRet)
                {
                    throw CException (-1, L"Table import failed.");
                }

            }   //  while (!feof (ioInstream))
    
            fclose (ioInStream);
        
            pProgress(100, false);

            return true;
        
        }   //  ImportTables (...)

        //
        //  Helpers
        //
        bool bCreateImportTable (const CEString& sTable, const CEString& sDescriptor, int iColumns)
        {
            CEString sSeparators(SZ_SEPARATOR);
            sSeparators += L", \n";
            CEString sHeader(sDescriptor);
            sHeader.ResetSeparators();
            sHeader.SetBreakChars(sSeparators);

            if (bTableExists(sTable))
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
                      bool bAutoincrement,
                      PROGRESS_CALLBACK_CLR pProgress)
        {
            long lFileLength = _filelength (_fileno (ioInstream))/sizeof (wchar_t);
            int iPercentDone = 0;

            CEString sStmt = L"INSERT INTO ";
            sStmt += sTable;
            sStmt += L" VALUES (";

            if (bAutoincrement)
            {
//                sStmt += L"(SELECT last_insert_rowid())";
                sStmt += L"NULL";
                for (int iCol = 1; iCol < iColumns; ++iCol)
                {
                    if (iCol > 0)
                    {
                        sStmt += L",";
                    }
                    sStmt += L"?";
                }
                sStmt += L")";
            }
            else
            {
                for (int iCol = 0; iCol < iColumns; ++iCol)
                {
                    if (iCol > 0)
                    {
                        sStmt += L",";
                    }
                    sStmt += L"?";
                }
                sStmt += L")";
            }

            sqlite3_stmt * pStmt = NULL;

            BeginTransaction();
       
            CEString sSeparators (SZ_SEPARATOR);
            sSeparators += L", \n";
            int iEntriesRead = 0;

            TCHAR szLineBuf[10000];
            CEString sLine;
            sLine.ResetSeparators();
            sLine.SetBreakChars(sSeparators);
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

                if (!bAutoincrement)
                {
                    __int64 llId = _wtoi64(sLine.sGetField(0));
                    Bind(1, llId, pStmt);
                }

                for (int iCol = 1; iCol < iColumns; ++iCol)
                {
                    Bind (iCol, sLine.sGetField (iCol), pStmt);
                }
            
                InsertRow (pStmt);
                Finalize (pStmt);

                int iPd = (int) (((double)iCharsRead/(double)lFileLength) * 100);
                if (iPd > iPercentDone)
                {
                    iPercentDone = min (iPd, 100);
                    pProgress (iPercentDone, false);
                }

            }   //  for (; !feof (ioInstream); ++iEntriesRead)

            CommitTransaction();

            return true;

        }   // Import (...)

        void ClearTable(const CEString& sTableName)
        {
            CEString sQuery(L"DELETE FROM ");
            sQuery += sTableName;
            Exec(sQuery);
        }

        int iLastID (const CEString& sTableName)     // returns the ID of the last entry in the table
        {
            int iLastId = 0;
            CEString sQuery = L"SELECT * FROM " + sTableName 
                + L" AS a0 WHERE NOT EXIST (SELECT * FROM " + sTableName 
                + L" AS a1 WHERE a1.id > a0.id)";
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
    
        }   // iLastID (...)

    };  //  class CSqlite

}   // namespace Hlib

#endif
