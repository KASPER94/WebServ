/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peanut <peanut@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 15:50:52 by skapersk          #+#    #+#             */
/*   Updated: 2024/10/20 19:51:11 by peanut           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

class conf {
    private:

    public:
        conf(const std::string &str);
        ~conf();
        conf(const conf &cpy);

        conf &operator=(const conf &rhs);
};