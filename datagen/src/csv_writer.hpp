/*
 * The implementation of the GTPC graph data generator was built on
 * Florian Wolf's implementation of the CH-benCHmark data generator
 * (https://db.in.tum.de/research/projects/CHbenCHmark/) and 
 * Alexander van Renen's implementation of the TPC-C data generator
 * (https://github.com/alexandervanrenen/tpcc-generator)
 * See the README file.
 */

#ifndef csv_writer_hpp_
#define csv_writer_hpp_

#include <iostream>
#include <array>
#include <fstream>

namespace csv {

struct Precision {
   Precision(int p) : p(p) {}
   int p;
};

static struct EndlStruct { // Not std way to it but really easy for here.
} endl;

class CsvWriter {
   std::ofstream out;
   bool firstWordInLine;

   void prePrint();
   CsvWriter &printString(const char *str, int len);
public:
   CsvWriter(const std::string &path);

   friend CsvWriter &operator<<(CsvWriter &csv, int64_t num);
   friend CsvWriter &operator<<(CsvWriter &csv, float num);
   template<unsigned long len>
   friend CsvWriter &operator<<(CsvWriter &csv, const std::array<char, len> &data) { return csv.printString(data.data(), len); }
   friend CsvWriter &operator<<(CsvWriter &csv, const std::string &str);
   friend CsvWriter &operator<<(CsvWriter &csv, EndlStruct);
   friend CsvWriter &operator<<(CsvWriter &csv, Precision);
};

}

#endif
