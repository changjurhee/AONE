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

#include "common/logger.h"

std::ofstream Logger::null_stream = std::ofstream("/dev/null");
std::shared_ptr<Logger> Logger::loggerInstance = std::shared_ptr<Logger>(new Logger());


/**
* Configure Logger Preferences
* @param logFile
* @param level: LogLevel::ERROR by Default
* @param output: LogOutput::CONSOLE by Default
* @return void
*/
void Logger::SetLogPreferences(std::string logFileName = "",
	LogLevel level = LogLevel::LL_ERROR,
	LogOutput output = LogOutput::CONSOLE) {

	std::lock_guard<std::mutex> lock(mtx_);
	logLevel = level;
	logOutput = output;

	if (logOutput == LogOutput::FILE && !logFileName.empty()) {
		logFile.open(logFileName);
		if (!logFile.good()) {
			std::cerr << "Can't Open Log File" << std::endl;
			logOutput = LogOutput::CONSOLE;
		}
	}
}

void Logger::SetLogLevel(LogLevel level) {
	std::lock_guard<std::mutex> lock(mtx_);
	logLevel = level;
}

/**
* Get Single Logger Instance or Create new Object if Not Created
* @return std::shared_ptr<Logger>
*/
std::shared_ptr<Logger> Logger::GetInstance() {
	//if (loggerInstance == nullptr) {
	//	loggerInstance = std::shared_ptr<Logger>(new Logger());
	//}

	return loggerInstance;
}

/**
* Log given message with defined parameters and generate message to pass on Console or File
* @param codeFile: __FILE__
* @param codeLine: __LINE__
* @param message: Log Message
* @param messageLevel: Log Level, LogLevel::LL_DEBUG by default
*/
bool Logger::Log(std::string codeFile, std::string func, int codeLine, std::string message, LogLevel messageLevel, bool newline) {
	std::lock_guard<std::mutex> lock(mtx_);
	if (messageLevel > logLevel)
		return false;

	std::string logType;
	//Set Log Level Name
	switch (messageLevel) {
	case LogLevel::LL_LOG:
		logType = "LOG:\t";
		break;
	case LogLevel::LL_DEBUG:
		logType = "DEBUG:\t";
		break;
	case LogLevel::LL_INFO:
		logType = "INFO:\t";
		break;
	case LogLevel::LL_WARN:
		logType = "WARN:\t";
		break;
	case LogLevel::LL_ERROR:
		logType = "ERROR:\t";
		break;
	default:
		logType = "NONE:\t";
		break;
	}

	std::size_t found = codeFile.find_last_of("\\");
	codeFile = codeFile.substr(found + 1);
	codeFile += "(" + std::to_string(codeLine) + "):" + func + "() ";
	message = logType + codeFile + message;

	LogMessage(message, newline);

	return true;
}

std::ostream& Logger::LogObj(std::string codeFile, std::string func, int codeLine, std::string message, LogLevel messageLevel = LogLevel::LL_DEBUG) {
	if (Log(codeFile, func, codeLine, message, messageLevel, false))
		return std::cout;
	else
		return null_stream;
}

/**
* Convert String Representation of Log Level to LogLevel Type
* @param logLevel : String log level
* @return LogLevel
*/
LogLevel Logger::GetLogLevel(const std::string& logLevel) {
	std::lock_guard<std::mutex> lock(mtx_);
	if (logLevel == "LOG") {
		return LogLevel::LL_LOG;
	}
	if (logLevel == "DEBUG") {
		return LogLevel::LL_DEBUG;
	}
	else if (logLevel == "INFO") {
		return LogLevel::LL_INFO;
	}
	else if (logLevel == "WARN") {
		return LogLevel::LL_ERROR;
	}
	else if (logLevel == "ERROR") {
		return LogLevel::LL_ERROR;
	}
	return LogLevel::LL_NONE;
}

/**
* Convert String Representation of Log Output to LogOutput Type
* @param logOutput : String log output
* @return LogOutput
*/
LogOutput Logger::GetLogOutput(const std::string& logOutput) {
	std::lock_guard<std::mutex> lock(mtx_);
	if (logOutput == "FILE") {
		return LogOutput::FILE;
	}
	//If corrupted string passed output will be on console
	return LogOutput::CONSOLE;
}

/**
 * Write out the message to target output
 * @param message : String message
 * @return void
 */
void Logger::LogMessage(const std::string& message, bool newline) {
	if (logOutput == LogOutput::FILE) {
		logFile << message;
		if (newline)
			logFile << std::endl;
	}
	else {
		std::cout << message;
		if (newline)
			std::cout << std::endl;
	}
}
/*
template<typename T>
std::ostream& Logger::operator<<(T p)
{
	//os << dt.mo << '/' << dt.da << '/' << dt.yr;
	std::cout << p;
	return std::cout;
}
*/
