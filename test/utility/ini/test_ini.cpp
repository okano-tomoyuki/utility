#include <fstream>
#include <string>
#include <cassert>

#include "utility/ini.hpp"

int main()
{
    using Utility::Ini;

    const std::string INI_FILE_PATH("example.ini");

    // テスト用iniファイルの作成
    {
        const char* sample_ini_string = 
            "[SECTION_A]\n"
            "INTEGER_FIELD = 1000\n"
            "DOUBLE_FIELD = 10.5\n"
            "STRING_FIELD = hoge\n"
            "BOOLEAN_FIELD = true\n"
            "[SECTION_B]\n"
            "INTEGER_FIELD = 150\n"
            "DOUBLE_FIELD = -2.466\n"
            "STRING_FIELD = fuga\n"
            "BOOLEAN_FIELD = FALSE\n"
            "[SECTION_C]\n"
            "INTEGER_FIELD = -30\n"
            "DOUBLE_FIELD = 2.3338\n"
            "STRING_FIELD = piyo\n"
            "BOOLEAN_FIELD = tRue\n";
        std::ofstream ofs(INI_FILE_PATH);
        ofs << sample_ini_string;
    }

    auto ini = Ini(INI_FILE_PATH);

    // 読込テスト 該当セクション・フィールド有の場合
    assert(1000   == ini.read_int("SECTION_A",     "INTEGER_FIELD",  -999));
    assert(10.5   == ini.read_double("SECTION_A",  "DOUBLE_FIELD", -999.9));
    assert("hoge" == ini.read_str("SECTION_A",     "STRING_FIELD",     ""));
    assert(true   == ini.read_bool("SECTION_A",    "BOOLEAN_FIELD", false));
    assert(150    == ini.read_int("SECTION_B",     "INTEGER_FIELD",  -999));
    assert(-2.466 == ini.read_double("SECTION_B",  "DOUBLE_FIELD", -999.9));
    assert("fuga" == ini.read_str("SECTION_B",     "STRING_FIELD",     ""));
    assert(false  == ini.read_bool("SECTION_B",    "BOOLEAN_FIELD",  true));
    assert(-30    == ini.read_int("SECTION_C",     "INTEGER_FIELD",  -999));
    assert(-2.338 == ini.read_double("SECTION_C",  "DOUBLE_FIELD", -999.9));
    assert("piyo" == ini.read_str("SECTION_C",     "STRING_FIELD",     ""));
    assert(true   == ini.read_bool("SECTION_C",    "BOOLEAN_FIELD", false));

    // 読込テスト 該当セクション・フィールド有の場合
    assert(-999   == ini.read_int("SECTION_D",     "INTEGER_FIELD",  -999));
    assert(-999.9 == ini.read_double("SECTION_D",  "DOUBLE_FIELD", -999.9));
    assert(""     == ini.read_str("SECTION_D",     "STRING_FIELD",     ""));
    assert(true   == ini.read_bool("SECTION_D",    "BOOLEAN_FIELD", false));
    assert(-999   == ini.read_int("SECTION_A",     "_INTEGER_FIELD_",  -999));
    assert(-999.9 == ini.read_double("SECTION_A",  "_DOUBLE_FIELD_", -999.9));
    assert(""     == ini.read_str("SECTION_A",     "_STRING_FIELD_",     ""));
    assert(true   == ini.read_bool("SECTION_A",    "_BOOLEAN_FIELD_", false));


}