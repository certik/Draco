//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   parser/test/tstParse_Table.cc
 * \author Kent G. Budge
 * \date   Feb 18 2003
 * \brief  Unit tests for the Parse_Table class.
 * \note   Copyright (C) 2016-2019 Triad National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//

#include "ds++/Release.hh"
#include "ds++/ScalarUnitTest.hh"
#include "parser/File_Token_Stream.hh"
#include "parser/Parse_Table.hh"
#include "parser/String_Token_Stream.hh"
#include <string.h>

#ifdef _MSC_VER
#undef ERROR
#endif

using namespace std;
using namespace rtt_parser;
using namespace rtt_dsxx;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

static const char *color[3] = {"BLACK", "BLUE", "BLUE GREEN"};
bool color_set[3];

static void Parse_Color(Token_Stream &, int i) {
  cout << "You have requested " << color[i] << endl;
  color_set[i] = true;
}

static void Parse_Any_Color(Token_Stream &tokens, int) {
  Token token = tokens.shift();
  for (unsigned i = 0; i < sizeof(color) / sizeof(const char *); i++)
    if (!strcmp(token.text().c_str(), color[i])) {
      cout << "You have requested " << color[i] << endl;
      color_set[i] = true;
      return;
    }

  tokens.report_syntax_error(token, "expected a color");
}

const Keyword raw_table[] = {
    {"BLUE", Parse_Color, 1, "main"},
    {"BLACK", Parse_Color, 0, "main"},
    {"BLUE GREEN", Parse_Color, 2, "main"},
    {"BLUISH GREEN", Parse_Color, 2, "main"},
    {"lower blue", Parse_Color, 2, "main", "keyword to test case sensitivity"},
    {"COLOR", Parse_Any_Color, 0, "main"},
};
const size_t raw_table_size = sizeof(raw_table) / sizeof(Keyword);

//---------------------------------------------------------------------------//
 void tstParse_Table(UnitTest &ut) {
  Parse_Table table;

  table.reserve(raw_table_size);
  table.add(raw_table, raw_table_size);

  // Build path for the input file "parser_test.inp"
  string const ptInputFile(ut.getTestSourcePath() +
                           std::string("parser_test.inp"));
  File_Token_Stream token_stream(ptInputFile);

  table.parse(token_stream);
  token_stream.rewind();
  table.set_flags(Parse_Table::CASE_INSENSITIVE);
  color_set[0] = color_set[1] = 0;
  table.parse(token_stream);

  String_Token_Stream tokens("lower blue");
  table.parse(tokens);
  if (tokens.error_count() != 0)
    FAILMSG("Did NOT match lower case");

  return;
 }

//---------------------------------------------------------------------------//
int main(int argc, char *argv[]) {
  ScalarUnitTest ut(argc, argv, release);
  try {
    tstParse_Table(ut);
  }
  UT_EPILOG(ut);
}

//---------------------------------------------------------------------------//
// end of tstParse_Table.cc
//---------------------------------------------------------------------------//
