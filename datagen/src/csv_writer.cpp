/*
 * The implementation of the GTPC graph data generator was built on
 * Florian Wolf's implementation of the CH-benCHmark data generator
 * (https://db.in.tum.de/research/projects/CHbenCHmark/) and 
 * Alexander van Renen's implementation of the TPC-C data generator
 * (https://github.com/alexandervanrenen/tpcc-generator)
 * See the README file.
 */

#include "csv_writer.hpp"

#include <iomanip>
#include <cstring>

namespace csv {

CsvWriter::CsvWriter(const std::string &path)
        : out(path), firstWordInLine(true) {
   if (!out.good()) {
      std::cout << "\nCannot create file: '" << path << "'." << std::endl;
      std::cout << "aborting..." << std::endl;
      exit(-1);
   }
}

void CsvWriter::prePrint() {
   if (firstWordInLine) {
      firstWordInLine = false;
   } else {
      out << '|';
   }
}

CsvWriter &CsvWriter::printString(const char *str, int len) {
   prePrint();
   out.write(str, std::min((size_t) len, strnlen(str, len)));
   return *this;
}

CsvWriter &operator<<(CsvWriter &csv, int64_t num) {
   csv.prePrint();
   csv.out << num;
   return csv;
}

CsvWriter &operator<<(CsvWriter &csv, float num) {
   csv.prePrint();
   csv.out << std::fixed << num;
   return csv;
}

CsvWriter &operator<<(CsvWriter &csv, const std::string &str) {
   return csv.printString(str.c_str(), str.size());
}

CsvWriter &operator<<(CsvWriter &csv, EndlStruct) {
   csv.out << "\n";
   csv.firstWordInLine = true;
   return csv;
}

CsvWriter &operator<<(CsvWriter &csv, Precision precision) {
   csv.out << std::setprecision(precision.p);
   return csv;
}

}
