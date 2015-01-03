#include <string>
#include <iostream>
#include <regex>

void test(std::string test_string) {
  std::string pitch, octave;
  std::smatch sm;
  std::regex e ("\([abcdefg][fs]?)(-?[0123456789]*)");
  std::cout << "matching '" << test_string << "': ";
  if (regex_match(test_string, sm, e)) {
    // match
    pitch = sm[1].str();
    octave = sm[2].str();
    std::cout << "size=" << sm.size() << " p=" << pitch << " o=" << octave << "\n";
  } else {
    // no match
    std::cout << "size=" << sm.size() << " (no match)\n";
  } 
}

  
int main( void ) {
  test("a");
  test("a-1");
  test("a0");
  test("a1");
  test("a10");
  test("af");
  test("af-1");
  test("af10");
  test("af10");
  test("as-1");
  test("as10");
  test("B10");

  return 0;
}
