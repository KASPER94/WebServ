/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:34:08 by peanut            #+#    #+#             */
/*   Updated: 2024/10/22 13:42:13 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include "utils.h"

void	ltrim(std::string &str) {
	int	i = 0;

	if (str.length() == 0)
		return ;
	while (isspace(str[i]))
		i++;
	str = str.substr(i, str.length());
}

void	rtrim(std::string &str) {
	int	i = str.length() - 1;

	if (str.length() == 0)
		return ;
	while (isspace(str[i]))
		i--;
	str = str.substr(0, i + 1);
}

std::vector<std::string> split_trim_conf(std::string str, std::string needle) {
	std::vector<std::string>	split ;
	size_t			start = 0 ;
	size_t			end = 0 ;
	std::string			subs ;
	std::string			remaining ;

	while ((end = str.find(needle, start)) != std::string::npos) {
		subs = str.substr(start, end - start);
		ltrim(subs);
		rtrim(subs);
		if (!subs.empty()) {
			split.push_back(subs);
		}
		start = end + needle.length();
	}
	remaining = str.substr(start);
	ltrim(remaining);
	rtrim(remaining);
	if (!remaining.empty()) {
		split.push_back(remaining);
	}
	return split;
}

