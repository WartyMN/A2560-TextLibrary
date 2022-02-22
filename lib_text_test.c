// unit testing framework
#include "minunit.h"

// project includes
#include "test_base.h"
#include "lib_general.h"
#include "lib_general_a2560.h"

// class being tested
#include "lib_text.h"

// A2650 includes

// variables needed for testing

extern Screen global_screen[2];



// static int foo = 0;
// static int bar = 0;
// static double dbar = 0.1;
// static const char* foostring = "Thisstring";
// static const unsigned char* newkey1 = (unsigned char*)"newkey1";
// static const unsigned char* newkey1case = (unsigned char*)"NewKey1";
// static const unsigned char* newkey2 = (unsigned char*)"newkey2";
// static const unsigned char* newvalue1 = (unsigned char*)"foo";
// static const unsigned char* newvalue2 = (unsigned char*)"BAR";
// static const unsigned char* boolkey1 = (unsigned char*)"mybool1";
// static const unsigned char* boolkey2 = (unsigned char*)"mybool2";
// static const unsigned char* boolkey3 = (unsigned char*)"mybool3";
// static const unsigned char* boolstring = (unsigned char*)"false";
// static const unsigned char* intkey = (unsigned char*)"myint";
// static const unsigned char* intstring = (unsigned char*)"12345";
// static int myint = 12345;

void text_test_setup(void)	// this is called EVERY test
{
// 	foo = 7;
// 	bar = 4;
// 	
}

void text_test_teardown(void)	// this is called EVERY test
{

}

MU_TEST(text_test_fill_text)
{
	//mu_assert( Text_FillCharMem(CHANNEL_A, 'Z'), "Could not fill character memory in channel A" );
	mu_assert( Text_FillCharMem(CHANNEL_B, 4), "Could not fill character memory in channel B" );
}

MU_TEST(text_test_fill_attr)
{
	//mu_assert( Text_FillAttrMem(CHANNEL_A, 127), "Could not fill attribute memory in channel A" );
	mu_assert( Text_FillAttrMem(CHANNEL_B, 148), "Could not fill attribute memory in channel B" );

		// 31=black on white
		// 64=dark blue on black
		// 96=dark cyan on black
		// 112=medium gray on black
		// 128=medium gray on black
		// 138=black on light green
		// 139=black on bright yellow
		// 140=gray? on medium blue
		// 141=gray? on pink
		// 142=gray? on light cyan

		// 143=black/gray? on white
		// 15=black on white
	
		// 144=red on black
		// 16=dark red on black
	
		// 145=light red on dark red
		// 17=dark red on dark red
	
		// 146=light red on medium green
		// 18=dark red on medium green
	
		// 147=light red on olive
		// 19=dark red on medium green?
	
		// 148=light red on dark blue
}

// MU_TEST(text_test_update_font)
// {
// 	mu_assert( Text_UpdateFontData(CHANNEL_B, (unsigned int*)0x000000), "Could not replace font data for channel B" );
// }
// 
// MU_TEST(text_test_show_font)
// {
// 	mu_assert( Text_ShowFontChars(CHANNEL_B), "Could not show font chars for channel B" );
// }



// MU_TEST(test_check)
// {
// 	mu_check(foo == 7);
// }
// 
// MU_TEST(test_check_fail)
// {
// 	mu_check(foo != 7);
// }
// 
// MU_TEST(test_assert)
// {
// 	mu_assert(foo == 7, "foo should be 7");
// }
// 
// MU_TEST(test_assert_fail)
// {
// 	mu_assert(foo != 7, "foo should be <> 7");
// }
// 
// MU_TEST(test_assert_int_eq)
// {
// 	mu_assert_int_eq(4, bar);
// }
// 
// MU_TEST(test_assert_int_eq_fail)
// {
// 	mu_assert_int_eq(5, bar);
// }
// 
// MU_TEST(test_assert_double_eq)
// {
// 	mu_assert_double_eq(0.1, dbar);
// }
// 
// MU_TEST(test_assert_double_eq_fail)
// {
// 	mu_assert_double_eq(0.2, dbar);
// }
// 
// MU_TEST(test_fail)
// {
// 	mu_fail("Fail now!");
// }
// 
// MU_TEST(test_string_eq){
// 	mu_assert_string_eq("Thisstring", foostring);
// }
// 
// MU_TEST(test_string_eq_fail){
// 	mu_assert_string_eq("Thatstring", foostring);
// }


MU_TEST_SUITE(text_test_suite)
{
	MU_SUITE_CONFIGURE(&text_test_setup, &text_test_teardown);

	MU_RUN_TEST(text_test_fill_text);
	MU_RUN_TEST(text_test_fill_attr);
//	MU_RUN_TEST(text_test_update_font);
//	MU_RUN_TEST(text_test_show_font);
	
// 	MU_RUN_TEST(test_check);
// 	MU_RUN_TEST(test_assert);
// 	MU_RUN_TEST(test_assert_int_eq);
// 	MU_RUN_TEST(test_assert_double_eq);
// 
// 	MU_RUN_TEST(test_check_fail);
// 	MU_RUN_TEST(test_assert_fail);
// 	MU_RUN_TEST(test_assert_int_eq_fail);
// 	MU_RUN_TEST(test_assert_double_eq_fail);
// 	
// 	MU_RUN_TEST(test_string_eq);
// 	MU_RUN_TEST(test_string_eq_fail);
// 
// 	MU_RUN_TEST(test_fail);
}

int Text_RunTests(void)
{
	MU_RUN_SUITE(text_test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
