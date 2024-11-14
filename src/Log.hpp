/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrigny <yrigny@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 13:44:05 by yrigny            #+#    #+#             */
/*   Updated: 2024/11/12 19:56:47 by yrigny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
# define LOG_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <map>

enum LogState
{
	ON,
	OFF,
};

enum LogLevel
{
	DEBUG,
	INFO,
	ERROR,
};

enum LogMode
{
	CONSOLE,
	OUTFILE,
};

class Log
{
	public:
		static LogState	state;
		static LogMode	mode;
		static const std::string	levelStr[3];
		Log();
		~Log();
		static std::string	GetCurrTime();
		static void		LogMsg(LogLevel level, std::string msg);
		static void		SetState(std::string state);
		static void		SetMode(std::string mode);

		template <typename T>
		static std::string ToString(T value)
		{
			std::stringstream ss;
			ss << value;
			return ss.str();
		}
};

#endif
