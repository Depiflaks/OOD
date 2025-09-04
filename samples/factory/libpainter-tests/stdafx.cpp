// stdafx.cpp : source file that includes just the standard includes
// libpainter-tests.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


// Сообщаем библиотеке boost test, что функция main будет предоставлена нами (для предварительного конфигурирования библиотеки), 
// а не сгенерирована boost-ом за нас
#define BOOST_TEST_NO_MAIN 
// подключаем реализацию boost test
#pragma warning (push, 3)
#pragma warning (disable:4702)
#include <boost/test/included/unit_test.hpp>
#pragma warning (pop)
