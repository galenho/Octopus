-- 日志等级
LogLevel = 
{
	LOG_LEVEL_DEBUG = 0,
	LOG_LEVEL_INFO  = 1,
	LOG_LEVEL_WARN  = 2,
	LOG_LEVEL_ERROR = 3
}

function GetCmd(bytes)
	return string.byte(bytes, 1) + string.byte(bytes, 2) * 256 + string.byte(bytes, 3) * 65535 + string.byte(bytes, 4) * 16777216
end
