#pragma once
#include <iostream>

#define NOMINMAX

#include <memory>
#include <string>
#include <locale>
#include <codecvt>


namespace SDL_RTI
{
	class StringConverter {

	public:
		static std::wstring towstr(const std::string& s) {
			if (s.empty()) {
				return std::wstring();
			}
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
   			return converter.from_bytes(s);
		}
		static std::string tostr(const std::wstring& s) {
			if (s.empty()) {
				return std::string();
			}
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    		return converter.to_bytes(s);
		}

		static unsigned short extractNumberFromString(const std::string& str) {
			std::string numberStr;

			for (char ch : str) {
				if (std::isdigit(ch)) {
					numberStr += ch;
				}
			}

			return static_cast<unsigned short>(std::stoi(numberStr));
		}
	};
}