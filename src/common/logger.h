/*
// Copyright (c) 2019 Onur Dundar
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

// Come from https://github.com/odundar/simple_cpp_logger/tree/master

// Usage
// 
// Logger::GetInstance()->SetLogLevel(LogLevel::LL_DEBUG);
// LOG_DEBUG("waiting something..."); // only string is accepable
// LOG_OBJ_DEBUG() << "waiging " << 10 << " seconds..."; // Use like std::cout

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <memory>
#include <fstream>
#include <mutex>
#include <fstream>

enum class LogLevel {
	LL_NONE = 0,
	LL_ERROR = 1,
	LL_WARN = 2,
	LL_INFO = 3,
	LL_DEBUG = 4,
	LL_LOG = 5
};

enum class LogOutput {
	CONSOLE,
	FILE
};

/**
* Logger Class Used to Output Details of Current Application Flow
*/

class Logger {
public:
	static std::ofstream null_stream;
	static std::shared_ptr<Logger> GetInstance();

	void SetLogPreferences(std::string logFileName,
		LogLevel level,
		LogOutput output);
	void SetLogLevel(LogLevel level);

	bool Log(std::string codeFile, std::string func, int codeLine, std::string message, LogLevel messageLevel, bool newline = true);
	std::ostream& LogObj(std::string codeFile, std::string func, int codeLine, std::string message, LogLevel messageLevel);

	LogOutput GetLogOutput(const std::string& logOutput);
	LogLevel GetLogLevel(const std::string& logLevel);

	//template<typename T> std::ostream& operator<<( T p);
	/*
	template<typename T>
	friend std::ostream& operator<<(Logger& logger, const T& p);
	*/

private:
	LogLevel logLevel;
	LogOutput logOutput;
	std::ofstream logFile;
	std::mutex mtx_;

	static std::shared_ptr<Logger> loggerInstance;

	void LogMessage(const std::string& message, bool newline);

};

/*
template<typename T>
std::ostream& operator<<(Logger& logger, const T& p) {
	std::cout << "TEST: " << p << std::endl;

	return std::cout;
}
*/

#define LOG_NONE(msg) Logger::GetInstance()->Log(__FILE__, __FUNCTION__, __LINE__, msg, LogLevel::LL_NONE)
#define LOG_ERROR(msg) Logger::GetInstance()->Log(__FILE__, __FUNCTION__, __LINE__, msg, LogLevel::LL_ERROR)
#define LOG_WARN(msg) Logger::GetInstance()->Log(__FILE__, __FUNCTION__, __LINE__, msg, LogLevel::LL_WARN)
#define LOG_INFO(msg) Logger::GetInstance()->Log(__FILE__, __FUNCTION__, __LINE__, msg, LogLevel::LL_INFO)
#define LOG_DEBUG(msg) Logger::GetInstance()->Log(__FILE__, __FUNCTION__, __LINE__, msg, LogLevel::LL_DEBUG)
#define LOG_LOG(msg) Logger::GetInstance()->Log(__FILE__, __FUNCTION__, __LINE__, msg, LogLevel::LL_LOG)

#define LOG_OBJ_NONE() Logger::GetInstance()->LogObj(__FILE__, __FUNCTION__, __LINE__, "", LogLevel::LL_NONE)
#define LOG_OBJ_ERROR() Logger::GetInstance()->LogObj(__FILE__, __FUNCTION__, __LINE__, "", LogLevel::LL_ERROR)
#define LOG_OBJ_WARN() Logger::GetInstance()->LogObj(__FILE__, __FUNCTION__, __LINE__, "", LogLevel::LL_WARN)
#define LOG_OBJ_INFO() Logger::GetInstance()->LogObj(__FILE__, __FUNCTION__, __LINE__, "", LogLevel::LL_INFO)
#define LOG_OBJ_DEBUG() Logger::GetInstance()->LogObj(__FILE__, __FUNCTION__, __LINE__, "", LogLevel::LL_DEBUG)
#define LOG_OBJ_LOG() Logger::GetInstance()->LogObj(__FILE__, __FUNCTION__, __LINE__, "", LogLevel::LL_LOG)

#endif