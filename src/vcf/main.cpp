/* 
 * File:   main.cpp
 * Author: Cristina Yenyxe Gonzalez Garcia <cyenyxe@ebi.ac.uk>
 *
 * Created on 19 November 2014, 15:54
 */

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include <boost/program_options.hpp>

#include "vcf/file_structure.hpp"
#include "vcf/validator.hpp"

namespace
{
    size_t const default_line_buffer_size = 64 * 1024;
    namespace po = boost::program_options;
    
    po::options_description build_command_line_options()
    {
        po::options_description description("Usage: vcf-validator [OPTIONS] [< input_file]\nAllowed options");
        
        description.add_options()
            ("help,h", "Display this help")
            ("level,l", po::value<std::string>()->default_value("warning"), "Validation level: error, warning, block")
            ("input,i", po::value<std::string>()->default_value("stdin"), "Path to the input VCF file, or stdin")
        ;
        
        return description;
    }
    
    int check_command_line_options(po::variables_map & vm, po::options_description & desc)
    {
        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return -1;
        }
        
        std::string level = vm["level"].as<std::string>();
        if (level != "error" && level != "warning" && level != "block") {
            std::cout << desc << std::endl;
            std::cout << "Please choose one of the accepted validation levels" << std::endl;
            return 1;
        }
        
        return 0;
    }
    
    template <typename Container>
    std::istream & readline(std::istream & stream, Container & container)
    {
        char c;
        bool not_eof;

        container.clear();

        do {
            not_eof = stream.get(c);
            container.push_back(c);
        } while (not_eof && c != '\n');

        return stream;
    }

    bool is_valid_vcf_file(std::istream & input, std::string & input_name)
    {
        auto source = opencb::vcf::Source{input_name, opencb::vcf::InputFormat::VCF_FILE_VCF};
        auto records = std::vector<opencb::vcf::Record>{};

        auto validator = opencb::vcf::FullValidator{
            std::make_shared<opencb::vcf::Source>(source),
            std::make_shared<std::vector<opencb::vcf::Record>>(records)};

        std::vector<char> line;
        line.reserve(default_line_buffer_size);

        while (readline(input, line)) { 
           validator.parse(line);
        }

        validator.end();

        return validator.is_valid();
    }
}

int main(int argc, char** argv)
{
    po::options_description desc = build_command_line_options();
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    int check_options = check_command_line_options(vm, desc);
    if (check_options < 0) { return 0; }
    if (check_options > 0) { return check_options; }
    
    bool is_valid;

    try {
        std::string path = vm["input"].as<std::string>();
    
        if (path == "stdin") {
            std::cout << "Reading from standard input..." << std::endl;
            is_valid = is_valid_vcf_file(std::cin, path);
        } else {
            std::cout << "Reading from input file..." << std::endl;
            std::ifstream input{path};
            is_valid = is_valid_vcf_file(input, path);
        }

        std::cout << "The input file is " << (is_valid ? "valid" : "not valid") << std::endl;
        return !is_valid; // A valid file returns an exit code 0
        
    } catch (std::runtime_error const & ex) {
        std::cout << "The input file is not valid" << std::endl;
        std::cerr << ex.what() << std::endl;
        return 1;
    }
}
