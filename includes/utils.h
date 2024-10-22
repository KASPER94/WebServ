/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:40:34 by peanut            #+#    #+#             */
/*   Updated: 2024/10/22 13:55:19 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

void	ltrim(std::string &str);
void	rtrim(std::string &str);
std::vector<std::string> split_trim_conf(std::string str, std::string needle);

#endif