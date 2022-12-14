/*************************************************************************
 *
 * Compliant Enterprise Logging
 *
 * Implementation file for logging interface described in celog.h
 *
 ************************************************************************/

#include "stdafx.h"

#include "common/celog2/celog.h"
#include "celog_mgr.h"

extern CELogMgr mgr;

_Check_return_
bool CELog_Init(void)
{
  return mgr.Init();
}

void CELog_Destroy(void)
{
  mgr.Destroy();
}

_Check_return_
bool CELog_IsPerfLogMode(void)
{
  return mgr.IsPerfLogMode();
}

int CELog_LogA(_In_     celog_filepathint_t fileInt,
               _In_     int line,
               _In_z_   const char *mod,
               _In_z_   const char *func,
               _In_     celog_level_t level,
               _In_z_ _Printf_format_string_ const char *fmt,
               _In_     ...)
{
  va_list argPtr;
  int ret;

  va_start(argPtr, fmt);
  ret = mgr.Log(fileInt, line, mod, func, level, fmt, argPtr);
  va_end(argPtr);

  return ret;
}

int CELog_LogW(_In_     celog_filepathint_t fileInt,
               _In_     int line,
               _In_z_   const char *mod,
               _In_z_   const char *func,
               _In_     celog_level_t level,
               _In_z_ _Printf_format_string_ const wchar_t *fmt,
               _In_     ...)
{
  va_list argPtr;
  int ret;

  va_start(argPtr, fmt);
  ret = mgr.Log(fileInt, line, mod, func, level, fmt, argPtr);
  va_end(argPtr);

  return ret;
}
