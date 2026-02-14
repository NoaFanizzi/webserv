/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DEBUG_PrintConfig.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 05:38:20 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/23 05:38:30 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>

void Config::printServers()
{
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           WEBSERV CONFIGURATION TREE                      ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n" << std::endl;
    
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        const ServerConfig &srv = _servers[i];
        
        std::cout << "📦 Server #" << (i + 1) << std::endl;
        std::cout << "├── 🔌 Port: " << srv.port << std::endl;
        std::cout << "├── 🌐 Host: " << srv.host << std::endl;
        std::cout << "├── 📁 Root: " << srv.root << std::endl;
        std::cout << "├── 📄 Index: " << srv.index << std::endl;
        std::cout << "├── 📂 Autoindex: " << (srv.autoindex ? "✓ on" : "✗ off") << std::endl;
        std::cout << "├── 📦 Max body size: " << srv.client_max_body_size << " bytes" << std::endl;
        
        // Error pages
        if (!srv.error_page.empty())
        {
            std::cout << "├── ⚠️  Error pages:" << std::endl;
            for (size_t j = 0; j < srv.error_page.size(); ++j)
            {
                bool isLast = (j == srv.error_page.size() - 1) && srv.locations.empty();
                std::cout << (isLast ? "│   └── " : "│   ├── ");
                std::cout << "Code " << srv.error_page[j].index 
                          << " → " << srv.error_page[j].path << std::endl;
            }
        }
        
        // Locations
        if (!srv.locations.empty())
        {
            std::cout << "└── 📍 Locations (" << srv.locations.size() << "):" << std::endl;
            for (size_t j = 0; j < srv.locations.size(); ++j)
            {
                const LocationConfig &loc = srv.locations[j];
                bool isLastLocation = (j == srv.locations.size() - 1);
                
                std::cout << "    " << (isLastLocation ? "└── " : "├── ") 
                          << "🎯 " << loc.path << std::endl;
                
                std::string prefix = isLastLocation ? "        " : "    │   ";
                
                if (!loc.root.empty())
                    std::cout << prefix << "├── Root: " << loc.root << std::endl;
                if (!loc.index.empty())
                    std::cout << prefix << "├── Index: " << loc.index << std::endl;
                
                std::cout << prefix << "├── Autoindex: " 
                          << (loc.autoindex ? "✓ on" : "✗ off") << std::endl;
                
                if (!loc.allowed_methods.empty())
                {
                    std::cout << prefix << "└── Methods: ";
                    for (size_t m = 0; m < loc.allowed_methods.size(); ++m)
                    {
                        std::cout << loc.allowed_methods[m];
                        if (m < loc.allowed_methods.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << std::endl;
                }
                
                if (j < srv.locations.size() - 1)
                    std::cout << "    │" << std::endl;
            }
        }
        
        if (i < _servers.size() - 1)
            std::cout << "\n" << std::endl;
    }
    
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Total servers: " << _servers.size() << "                                         ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n" << std::endl;
}
