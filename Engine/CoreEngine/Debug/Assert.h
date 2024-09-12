#pragma once
#include <comdef.h>
#include <source_location>

#include "common_include.h"
#include "Core/Utils/Logger.h"


/**
 * 반환 된 HResult값을 판단 Succeeded에서 실패하면 프로그램 자동 종료 및 로그 남김
 * @param InResult HResult 반환 값
 * @param InLocation 이 메서드 호출 위치
 */
inline void CheckResult(HRESULT InResult, const std::source_location& InLocation = std::source_location::current())
{
	if (!SUCCEEDED(InResult))
	{
		_com_error err(InResult);
		LPCTSTR    errMsg = err.ErrorMessage();

		LOG_CORE_ERROR("Function:{0}, Line:{1}, Error:{2}",
					   InLocation.file_name(),
					   InLocation.line(),
					   WString2String( errMsg));

		assert(false);
	}
}
