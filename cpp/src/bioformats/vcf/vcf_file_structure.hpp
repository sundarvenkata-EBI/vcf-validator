/* 
 * File:   vcf_file_structure.hpp
 * Author: Cristina Yenyxe Gonzalez Garcia <cyenyxe@ebi.ac.uk>
 *
 * Created on 19 November 2014, 10:12
 */

#ifndef OPENCB_VCF_VCFFILESTRUCTURE_H
#define	OPENCB_VCF_VCFFILESTRUCTURE_H

#include <algorithm>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace opencb
{
  namespace vcf
  {
    
    struct VcfMetaEntry 
    {
        std::string id;
        std::string plain_value;
        std::map<std::string, std::string> key_values;
        
        VcfMetaEntry(std::string const & id,
                  std::string const & plain_value);
        
        VcfMetaEntry(std::string const & id,
                  std::map<std::string, std::string> const & key_values);
    };
    
    enum class VcfInputFormat 
    {
        VCF_FILE_VCF    = 0x01,
        VCF_FILE_GVCF   = 0x02,
        VCF_FILE_GZIP   = 0x04,
        VCF_FILE_BGZIP  = 0x08,
        VCF_FILE_BCF    = 0x10,
    };
    
    struct VcfSource 
    {
        std::string name;       /**< Name of the source to interact with (file, stdin...) */
        char mode;              /**< Mode for interaction (w/r/a) */
        
        std::string version;    /**< VCF version */
        unsigned input_format;  /**< Mask that stores whether the input is plain/gzipped VCF, BCF, etc */

        std::vector<VcfMetaEntry> meta_entries;    /**< Entries in the file meta-data */
        std::vector<std::string> samples_names;    /**< Names of the sequenced samples */
        
        VcfSource(std::string const & name,
                  char const mode,
                  unsigned const input_format,
                  std::string const & version,
                  std::vector<VcfMetaEntry> const & meta_entries,
                  std::vector<std::string> const & samples_names);
    };
    
    struct VcfRecord 
    {
        std::string chromosome;
        size_t position;
        std::vector<std::string> ids;

        std::string reference_allele;
        std::vector<std::string> alternate_alleles;

        float quality;
        std::vector<std::string> filters;
        std::string info;
        std::string format;

        std::vector<std::string> samples;
        
        std::shared_ptr<VcfSource> source;
        

        VcfRecord(std::string const & chromosome, 
                  size_t const position, 
                  std::vector<std::string> const & ids, 
                  std::string const & reference_allele, 
                  std::vector<std::string> const & alternate_alleles, 
                  float const quality, 
                  std::vector<std::string> const & filters, 
                  std::string const & info, 
                  std::string const & format, 
                  std::vector<std::string> const & samples,
                  std::shared_ptr<VcfSource> const & source);

        bool operator==(VcfRecord const &);

        bool operator!=(VcfRecord const &);
        
    private:
        /**
         * Checks that chromosome does not contain colons or white-spaces
         * 
         * @throw std::invalid_argument
         */
        void check_chromosome();
        
        /**
         * Checks that IDs are alphanumeric
         * 
         * @throw std::invalid_argument
         */
        void check_ids();
        
        /**
         * Checks that alternate and reference alleles share the first nucleotide 
         * when they have the same length, but are not exactly the same
         * 
         * @throw std::invalid_argument
         */
        void check_alternate_alleles();
        
        /**
         * Checks that quality is zero or greater
         * 
         * @throw std::invalid_argument
         */
        void check_quality();
        
        /**
         * Checks that format starts with GT
         * 
         * @throw std::invalid_argument
         */
        void check_format();
    };

  }
}

#endif	/* OPENCB_VCF_VCFFILESTRUCTURE_H */

