/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */


// Generic C
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <cmath>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <elf_parser.h>
#include <func_memory.h>


using namespace std;


FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size_bits,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    this->addr_size     = addr_size_bits;
    this->page_num_size    = page_bits;
    this->offset_size   = offset_bits;

    this->max_set_number = pow( 2, addr_size_bits - page_bits - offset_bits);
    this->max_pages_per_set = pow( 2, page_bits);
    this->max_page_size = pow( 2, offset_bits);

    this->content = new Set*[ this->max_set_number];
    for( size_t iterator = 0; iterator < ( this->max_set_number); iterator++)
        ( this->content)[ iterator] = NULL;


    // extract all ELF sections into the section_array variable
    vector<ElfSection> sections_array;
    //ElfSection::getAllElfSections( executable_file_name, sections_array); //*********

    // print the information about each section
    for ( int i = 0; i < sections_array.size(); ++i)
        cout << sections_array[ i].dump() << endl;

}

FuncMemory::~FuncMemory()
{
    for( size_t iterator = 0; iterator < ( this->max_set_number); iterator++)
        delete [] content[ iterator];
    delete [] this->content;
}

uint64 FuncMemory::startPC() const
{
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    MemLocation memlock( addr,
                         this->addr_size,
                         this->page_num_size,
                         this->offset_size);


    if( content[ memlock.set_num] == NULL)
    {
        cerr << "ERROR: unable to read "
             << " - the set does not exist" << endl;
        exit( EXIT_FAILURE);
    }

    Set* set = content[ memlock.set_num];
    return set->read( memlock.page_num, memlock.byte_num, num_of_bytes);
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    MemLocation memlock( addr,
                         this->addr_size,
                         this->page_num_size,
                         this->offset_size);

    if( content[ memlock.set_num] == NULL)
    {
        content[ memlock.set_num] = new Set( memlock.set_num, this->max_pages_per_set, this->max_page_size);
    }

    content[ memlock.set_num]->write( value, memlock.page_num, memlock.byte_num, num_of_bytes);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;

    oss << indent << "Dump memory section \"" << endl
        << indent << "  Content:" << endl;

    for( size_t iterator = 0; iterator < ( this->max_set_number); iterator++)
    {
        if( this->content[ iterator] != NULL)
        {
            oss << (this->content)[ iterator]->dump( "  ");
        }
    }

    return oss.str();
}

