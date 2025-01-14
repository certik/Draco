//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/test/tstPacking_Utils.cc
 * \author Thomas M. Evans
 * \date   Wed Nov  7 15:58:08 2001
 * \brief  Test the routines used for serializing and de-serializing C++
 *         objects.
 * \note   Copyright (C) 2016-2019 Triad National Security, LLC.
 *         All rights reserved */
//---------------------------------------------------------------------------//

#include "ds++/Packing_Utils.hh"
#include "ds++/Release.hh"
#include "ds++/ScalarUnitTest.hh"
#include "ds++/Soft_Equivalence.hh"
#include <sstream>

using namespace std;

using rtt_dsxx::pack_data;
using rtt_dsxx::Packer;
using rtt_dsxx::soft_equiv;
using rtt_dsxx::unpack_data;
using rtt_dsxx::Unpacker;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void do_some_packing(Packer &p, vector<double> const &vd,
                     vector<int> const &vi) {
  for (const auto &item : vd)
    p << item;
  for (const auto &item : vi)
    p << item;
  return;
}

//---------------------------------------------------------------------------//
void compute_buffer_size_test(rtt_dsxx::UnitTest &ut) {
  // make data

  vector<double> vd = {2.3432, 2.3432, 2.3432, 22.4, 2.3432};
  vector<int> vi = {7, 22, 6};
  char const test_string[] = "test";
  size_t const total_size = vi.size() * sizeof(int) +
                            vd.size() * sizeof(double) + sizeof(test_string);

  // includes one padding byte
  Packer p;

  // Compute the required buffer size.

  p.compute_buffer_size_mode();
  do_some_packing(p, vd, vi); // computes the size

  p.pad(1);
  p.accept(4, test_string);
  FAIL_IF_NOT(total_size == p.size());

  Check(p.size() < UINT_MAX);
  vector<char> buffer(static_cast<unsigned>(p.size()));

  // Pack into buffer.

  p.set_buffer(p.size(), &buffer[0]);
  do_some_packing(p, vd, vi); // actually does the packing

  p.pad(1);
  p.accept(4, test_string);

  FAIL_IF_NOT(static_cast<int>(total_size) == p.end() - p.begin());

  // Unpack

  Unpacker u;
  u.set_buffer(p.size(), &buffer[0]);
  FAIL_IF_NOT(static_cast<int>(u.size()) == u.end() - u.begin());

  for (double &val : vd) {
    double d(-42.42);
    u >> d;
    FAIL_IF_NOT(soft_equiv(d, val));
  }

  for (int const val : vi) {
    int j(-42);
    u >> j;
    FAIL_IF_NOT(j == val);
  }

  // padding byte
  u.skip(1);

  for (unsigned i = 0; i < 4; ++i) {
    char c;
    u >> c;
    FAIL_IF_NOT(c == test_string[i]);
  }

  // Now test the global function pack_vec_double.
  {
    auto buffer_size(static_cast<unsigned>(vd.size() * sizeof(double)));
    vector<char> lbuffer(buffer_size);
    bool byte_swap = false;
    Check(vd.size() < UINT32_MAX);
    rtt_dsxx::pack_vec_double(&vd[0], &lbuffer[0],
                              static_cast<uint32_t>(vd.size()), byte_swap);

    Unpacker localUnpacker;
    localUnpacker.set_buffer(lbuffer.size(), &lbuffer[0]);

    if (static_cast<int>(localUnpacker.size()) !=
        localUnpacker.end() - localUnpacker.begin())
      ITFAILS;

    for (double const val : vd) {
      double d(-42.42);
      localUnpacker >> d;
      FAIL_IF_NOT(soft_equiv(d, val));
    }
  }

  if (ut.numFails == 0)
    PASSMSG("compute_buffer_size_test() worked fine.");

  return;
}

//---------------------------------------------------------------------------//
void packing_test(rtt_dsxx::UnitTest &ut) {

  double const eps = std::numeric_limits<double>::epsilon();
  double const mrv = std::numeric_limits<double>::min();

  // make some data
  double x = 102.45;
  double y = 203.89;
  double z = 203.88;

  int ix = 10;
  int iy = 11;
  int iz = 12;

  // make 2 buffers for data
  int s1 = 2 * sizeof(double) + 2 * sizeof(int);
  auto *b1 = new char[s1];
  int s2 = sizeof(double) + sizeof(int);
  auto *b2 = new char[s2];

  // pack the data
  {
    Packer p;

    p.set_buffer(s1, b1);
    p << x << ix;
    p.pack(y);
    p.pack(iy);

    if (p.get_ptr() != b1 + s1)
      ITFAILS;

    p.set_buffer(s2, b2);
    p << iz << z;

    if (p.get_ptr() != b2 + s2)
      ITFAILS;

    // Catch a failure when excedding the buffer limit:
    if (ut.dbcOn() && !ut.dbcNothrow()) {
      bool caught = false;
      try {
        p << iz;
      } catch (const rtt_dsxx::assertion & /* error */) {
        cout << "Good, caught the exception" << endl;
        caught = true;
      }
      if (!caught)
        ITFAILS;
    }
  }

  // unpack the data
  {
    Unpacker u;

    double d = 0;
    int i = 0;

    u.set_buffer(s1, b1);
    u >> d >> i;
    if (!soft_equiv(d, 102.45, eps))
      ITFAILS;
    if (i != 10)
      ITFAILS;

    u.unpack(d);
    u.unpack(i);
    if (!soft_equiv(d, 203.89, eps))
      ITFAILS;
    if (i != 11)
      ITFAILS;

    if (u.get_ptr() != s1 + b1)
      ITFAILS;

    // If DBC is off or if DBC nothrow is on, then this test is invalid.
    if (ut.dbcOn() && !ut.dbcNothrow()) {
      // try catching a failure
      bool caught = false;
      try {
        u.unpack(i);
      } catch (const rtt_dsxx::assertion & /* error */) {
        cout << "Good, caught the exception" << endl;
        caught = true;
      }
      if (!caught)
        ITFAILS;
    }

    u.set_buffer(s2, b2);
    u >> i >> d;
    if (i != 12)
      ITFAILS;
    if (!rtt_dsxx::soft_equiv(d, 203.88))
      ITFAILS;

    if (u.get_ptr() != s2 + b2)
      ITFAILS;
  }

  delete[] b1;
  delete[] b2;

  // try packing a vector and char array
  double r = 0;
  srand(125);

  vector<double> vx(100, 0.0);
  vector<double> ref(100, 0.0);

  char c[4] = {'c', 'h', 'a', 'r'};

  for (size_t i = 0; i < vx.size(); i++) {
    r = rand();
    vx[i] = r / RAND_MAX;
    ref[i] = vx[i];
  }

  int size = 100 * sizeof(double) + 4;
  auto *buffer = new char[size];

  // pack
  {
    Packer p;
    p.set_buffer(size, buffer);

    for (double const val : vx)
      p << val;

    for (char const val : c)
      p << val;

    if (p.get_ptr() != buffer + size)
      ITFAILS;
  }

  // unpack
  {
    char cc[4];
    vector<double> lx(100, 0.0);

    Unpacker u;
    u.set_buffer(size, buffer);

    for (double &val : lx)
      u >> val;

    u.extract(4, cc);

    if (u.get_ptr() != buffer + size)
      ITFAILS;

    if (!rtt_dsxx::soft_equiv(lx.begin(), lx.end(), ref.begin(), ref.end()))
      ITFAILS;

    if (c[0] != 'c')
      ITFAILS;
    if (c[1] != 'h')
      ITFAILS;
    if (c[2] != 'a')
      ITFAILS;
    if (c[3] != 'r')
      ITFAILS;
  }

  // Skip some data and unpack
  {
    char cc[2];
    vector<double> lx(100, 0.0);

    Unpacker u;
    u.set_buffer(size, buffer);

    // Skip the first 50 integers.
    u.skip(50 * sizeof(double));
    for (size_t i = 50; i < lx.size(); ++i)
      u >> lx[i];

    // Skip the first two chatacters
    u.skip(2);
    u.extract(2, cc);

    for (size_t i = 0; i < 50; ++i)
      if (!rtt_dsxx::soft_equiv(lx[i], 0.0, mrv))
        ITFAILS;
    for (size_t i = 50; i < lx.size(); ++i)
      if (!rtt_dsxx::soft_equiv(lx[i], ref[i], eps))
        ITFAILS;

    if (cc[0] != 'a')
      ITFAILS;
    if (cc[1] != 'r')
      ITFAILS;
  }

  delete[] buffer;
}

//---------------------------------------------------------------------------//
void packing_test_c90(rtt_dsxx::UnitTest &ut) {
  using std::vector;

  std::cout << "\nTesting packing/unpacking size_t and uint64_t..."
            << std::endl;
  size_t const numFails(ut.numFails);

  // make some data
  double x = 102.45;
  double y = 203.89;
  double z = 203.88;

  size_t ix = 10;
  uint64_t iy = 11;
  size_t iz = 12;

  // make 2 buffers for data
  size_t s1 = 2 * sizeof(double) + sizeof(size_t) + sizeof(uint64_t);
  //char *b1  = new char[s1];
  vector<char> b1(s1);
  size_t s2 = sizeof(double) + sizeof(size_t);
  //char *b2  = new char[s2];
  vector<char> b2(s2);

  // pack the data
  {
    Packer p;

    p.set_buffer(s1, &b1[0]);
    p << x << ix;
    p.pack(y);
    p.pack(iy);

    if (p.get_ptr() != &b1[0] + s1)
      ITFAILS;

    p.set_buffer(s2, &b2[0]);
    p << iz << z;

    if (p.get_ptr() != &b2[0] + s2)
      ITFAILS;
  }

  // unpack the data
  {
    Unpacker u;

    double d = 0;
    size_t i = 0;
    uint64_t i64 = 0;

    u.set_buffer(s1, &b1[0]);
    u >> d >> i;
    if (!rtt_dsxx::soft_equiv(d, x))
      ITFAILS;
    if (i != ix)
      ITFAILS;

    u.unpack(d);
    u.unpack(i64);
    if (!rtt_dsxx::soft_equiv(d, y))
      ITFAILS;
    if (i64 != iy)
      ITFAILS;

    if (u.get_ptr() != s1 + &b1[0])
      ITFAILS;

    u.set_buffer(s2, &b2[0]);
    u >> i >> d;
    if (i != iz)
      ITFAILS;
    if (!rtt_dsxx::soft_equiv(d, z))
      ITFAILS;

    if (u.get_ptr() != s2 + &b2[0])
      ITFAILS;
  }

  if (numFails == ut.numFails) // no new failures.
    PASSMSG("Packing/unpacking size_t and uint64_t works.");
  else
    FAILMSG("Packing/unpacking size_t and uint64_t failed.");

  return;
}

//---------------------------------------------------------------------------//
void std_string_test(rtt_dsxx::UnitTest &ut) {
  vector<char> pack_string;

  {
    // make a string
    string hw("Hello World");

    // make a packer
    Packer packer;

    // make a char to write the string into
    pack_string.resize(hw.size() + 1 * sizeof(int));

    packer.set_buffer(pack_string.size(), &pack_string[0]);

    packer << static_cast<int>(hw.size());

    // pack it
    for (char &it : hw)
      packer << it;

    if (packer.get_ptr() != &pack_string[0] + pack_string.size())
      ITFAILS;
    if (packer.get_ptr() != packer.begin() + pack_string.size())
      ITFAILS;
  }

  // now unpack it
  Unpacker unpacker;
  unpacker.set_buffer(pack_string.size(), &pack_string[0]);

  // unpack the size of the string
  int size;
  unpacker >> size;

  string nhw;
  nhw.resize(size);

  // unpack the string
  for (char &val : nhw)
    unpacker >> val;

  FAIL_IF_NOT(unpacker.get_ptr() == &pack_string[0] + pack_string.size());

  // test the unpacked string
  string hw("Hello World");

  if (hw == nhw) {
    ostringstream message;
    message << "Unpacked string " << nhw << " that matches original "
            << "string " << hw;
    PASSMSG(message.str());
  } else {
    ostringstream message;
    message << "Failed to unpack string " << hw << " correctly. Instead "
            << "unpacked " << nhw;
    FAILMSG(message.str());
  }
}

//---------------------------------------------------------------------------//
void packing_functions_test(rtt_dsxx::UnitTest &ut) {

  // Data to pack:
  // -------------
  vector<double> x(5);
  string y("The quick brown fox jumps over the lazy dog.");

  for (int i = 0; i < 5; ++i)
    x[i] = 100.0 * static_cast<double>(i) + 2.5;

  // Pack the data
  // -------------

  vector<char> packed_vector;
  vector<char> packed_string;

  pack_data(x, packed_vector);
  pack_data(y, packed_string);

  if (packed_vector.size() != 5 * sizeof(double) + sizeof(int))
    ITFAILS;
  if (packed_string.size() != y.size() + sizeof(int))
    ITFAILS;

  /* We now pack the two packed datums (x and s) together by manually inserting
   * the data, including the size of the already packed arrays, into a new
   * array.
   *
   * Honestly, I can't think of a better demonstration of how brain-dead the
   * packing system is. The root cause is the inistance of only packing and
   * unpacking into vector<char>. These manage their own memory, so any attempt
   * to put non-trivial objects together in a data stream will require copying
   * data. We need the size of the packed data (which already contains size
   * data) because we have to reserve space in a vector<char> when we unpack.
   *
   * The repeated steps for the two vectors performed below could be factored
   * out into a function, but this is another layer of abstraction which
   * shouldn't be necessary. Furthermore, it wouldn't fix all of the extra
   * copies that this approach requires.
   *
   * I was able to improve the unpacking step somewhat with the addition of the
   * extract command in the unpacker. At least we don't have to spoon the packed
   * data between containers one character at a time any more.
   *
   * Someday, I'll fix this mess. But right now, I've got work to do.
   *
   *                                   -MWB.
   */

  // A place the hold the packed sizes of already packed data.
  vector<char> packed_int(sizeof(int));
  Packer p;

  // Storage for the combined packed data.
  vector<char> total_packed;

  p.set_buffer(sizeof(int), &packed_int[0]);

  // Pack the size of the packed vector into packed_int;
  p << static_cast<int>(packed_vector.size());

  // Now, manually copy the packed size of the packed vector into the total
  // data array.
  copy(packed_int.begin(), packed_int.end(), back_inserter(total_packed));

  // Now, manually append the data of the packed vector into the total data
  // array.
  copy(packed_vector.begin(), packed_vector.end(), back_inserter(total_packed));

  /* Now, we repeat the process for the string data */

  // Reset the packer, to re-use the space for holding packed data sizes:
  p.set_buffer(sizeof(int), &packed_int[0]);

  // Pack the size of the packed string.
  p << static_cast<int>(packed_string.size());

  // Manually copy the packed size of the packed string into the total data
  // array.
  copy(packed_int.begin(), packed_int.end(), back_inserter(total_packed));

  // Manually append the data of the packed string into the total data
  // array.
  copy(packed_string.begin(), packed_string.end(), back_inserter(total_packed));

  // Unpack the data
  // ---------------

  vector<double> x_new;
  string y_new;

  // Holding place for the size data.
  int size;

  Unpacker u;
  u.set_buffer(total_packed.size(), &total_packed[0]);

  u >> size;
  vector<char> packed_vector_new(size);
  u.extract(size, packed_vector_new.begin());

  u >> size;
  vector<char> packed_string_new(size);
  u.extract(size, packed_string_new.begin());

  if (u.get_ptr() != &total_packed[0] + total_packed.size())
    ITFAILS;

  unpack_data(x_new, packed_vector_new);
  unpack_data(y_new, packed_string_new);

  // Compare the results
  // -------------------

  if (!soft_equiv(x_new.begin(), x_new.end(), x.begin(), x.end()))
    ITFAILS;

  if (y_new != y)
    ITFAILS;

  if (ut.numFails == 0)
    PASSMSG("pack_data and unpack_data work fine.");

  return;
}

//---------------------------------------------------------------------------//
void endian_conversion_test(rtt_dsxx::UnitTest &ut) {

  Packer p;
  Unpacker up(true);

  // Test the int type.
  const int moo = 0xDEADBEEF;
  const int length = sizeof(int);

  // Pack
  char data[length];
  p.set_buffer(length, data);
  p << moo;

  // Unpack
  int oom = 0;
  up.set_buffer(length, data);
  up >> oom;

  // Check
  if (static_cast<unsigned>(oom) != 0xEFBEADDE)
    ITFAILS;

  // Verify that char data (being one byte) is unchanged.
  const char letters[] = "abcdefg";
  const int letter_length = sizeof(letters) / sizeof(char);

  // Pack
  char letter_data[letter_length];
  p.set_buffer(letter_length, letter_data);
  for (char const letter : letters)
    p << letter;

  // Unpack
  char unpacked_letters[letter_length];
  up.set_buffer(letter_length, letter_data);
  for (char &unpacked_letter : unpacked_letters)
    up >> unpacked_letter;

  // Check
  for (int i = 0; i < letter_length; ++i)
    FAIL_IF_NOT(unpacked_letters[i] == letters[i]);

  return;
}

//---------------------------------------------------------------------------//
void packing_map_test(rtt_dsxx::UnitTest &ut) {
  std::cout << "\nTesting packing/unpacking std::maps..." << std::endl;

  { // test packing/unpacking a simple map
    size_t const numFails(ut.numFails);

    std::map<int, int> mymap;
    mymap[3] = 33;
    mymap[1] = 11;
    mymap[4] = 44;

    // packed storage
    std::vector<char> packed_mymap;
    rtt_dsxx::pack_data(mymap, packed_mymap);

    // unpack the data
    std::map<int, int> mymap_new;
    rtt_dsxx::unpack_data(mymap_new, packed_mymap);

    // Check size
    if (mymap.size() != mymap_new.size())
      ITFAILS;
    // Check keys
    if (mymap_new.count(3) != 1)
      ITFAILS;
    if (mymap_new.count(1) != 1)
      ITFAILS;
    if (mymap_new.count(4) != 1)
      ITFAILS;
    // Check data per key.
    if (mymap_new[3] != mymap[3])
      ITFAILS;
    if (mymap_new[1] != mymap[1])
      ITFAILS;
    if (mymap_new[4] != mymap[4])
      ITFAILS;

    if (ut.numFails == numFails) // no new failures.
      PASSMSG("packing/unpacking std::map<T1,T2>");
    else
      FAILMSG("packing/unpacking std::map<T1,T2>");
  }

  { // test packing/unpacking a map of vectors.
    size_t const numFails(ut.numFails);

    std::map<int, std::vector<int>> mymap;
    mymap[3] = std::vector<int>(3, 33);
    mymap[1] = std::vector<int>(6, 121);
    mymap[4] = std::vector<int>(2, 1);

    // packed storage
    std::vector<char> packed_mymap;
    rtt_dsxx::pack_data(mymap, packed_mymap);

    // unpack the data
    std::map<int, std::vector<int>> mymap_new;
    rtt_dsxx::unpack_data(mymap_new, packed_mymap);

    // Check size
    if (mymap.size() != mymap_new.size())
      ITFAILS;
    // Check keys
    if (mymap_new.count(3) != 1)
      ITFAILS;
    if (mymap_new.count(1) != 1)
      ITFAILS;
    if (mymap_new.count(4) != 1)
      ITFAILS;
    // Check data per key.
    if (mymap_new[3] != mymap[3])
      ITFAILS;
    if (mymap_new[1] != mymap[1])
      ITFAILS;
    if (mymap_new[4] != mymap[4])
      ITFAILS;

    if (ut.numFails == numFails) // no new failures.
      PASSMSG("packing/unpacking std::map<T1,std::vector<T2>>");
    else
      FAILMSG("packing/unpacking std::map<T1,std::vector<T2>>");
  }

  return;
}

//---------------------------------------------------------------------------//
int main(int argc, char *argv[]) {
  rtt_dsxx::ScalarUnitTest ut(argc, argv, rtt_dsxx::release);
  try {
    packing_test(ut);
    packing_test_c90(ut);
    std_string_test(ut);
    packing_functions_test(ut);
    compute_buffer_size_test(ut);
    endian_conversion_test(ut);
    packing_map_test(ut);
  }
  UT_EPILOG(ut);
}

//---------------------------------------------------------------------------//
// end of tstPacking_Utils.cc
//---------------------------------------------------------------------------//
