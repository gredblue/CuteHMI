#include <cutehmi/functions.hpp>

#include <cutehmi/test/random.hpp>

#include <QtTest/QtTest>

namespace cutehmi {

class test_functions:
	public QObject
{
		Q_OBJECT

	private slots:
		void testApe();

		void testAle();

		void testClt();

		void testCgt();
};

void test_functions::testApe()
{
	QCOMPARE(ape(0.0, 1.0e-100, 0.25), true);

	QCOMPARE(ape(0.0, 0.24, 0.25), true);
	QCOMPARE(ape(0.0, 0.25, 0.25), true);
	QCOMPARE(ape(0.0, 0.26, 0.25), false);

	QCOMPARE(ape(1.0, 1.49, 0.25), true);
	QCOMPARE(ape(1.0, 1.51, 0.25), false);
	QCOMPARE(ape(1.0, 1.99, 0.25), false);
	QCOMPARE(ape(1.0, 2.0, 0.25), true);	// Weirdness coming from the fact that <= is used in comparison.
	QCOMPARE(ape(1.0, 2.01, 0.25), false);

	QCOMPARE(ape(1.01, 1.5, 0.25), true);
	QCOMPARE(ape(1.01, 1.52, 0.25), false);
	QCOMPARE(ape(1.01, 1.99, 0.25), false);
	QCOMPARE(ape(1.01, 2.0, 0.25), true);
	QCOMPARE(ape(1.01, 2.01, 0.25), true);

	QCOMPARE(ape(0.0, 0.0), true);
	QCOMPARE(ape(0.5, 0.5), true);
	QCOMPARE(ape(0.5e16, 0.5e16), true);
	QCOMPARE(ape(0.0, 0.0625), false);
	QCOMPARE(ape(0.5, 0.5 + 0.0625), false);

	qreal r = test::randExp<qreal>(0, std::numeric_limits<qreal>::max_exponent);
	QCOMPARE(ape(r, r + std::numeric_limits<qreal>::epsilon() * 0.5), true);
	QCOMPARE(ape(r, 1.0625 * r), false);
}

void test_functions::testAle()
{
	QCOMPARE(ale(0.0, 1.0e-100, 0.25), false);	// Problematic behavior of ale() function.

	QCOMPARE(ale(1.0, 1.125, 0.25), true);
	QCOMPARE(ale(1.0, 1.99, 0.25), false);
	QCOMPARE(ale(1.0, 2.0, 0.25), false);
	QCOMPARE(ale(1.0, 2.01, 0.25), false);
	QCOMPARE(ale(2.0, 2.01, 0.25), true);
}

void test_functions::testClt()
{
	QCOMPARE(clt(0.0, 0.0, 0.25), false);
	QCOMPARE(clt(-0.125, 0.0, 0.25), false);
	QCOMPARE(clt(-0.5, 0.0, 0.25), true);

	QCOMPARE(clt(-1000, -100, 0.25), true);
	QCOMPARE(clt(-1000, -999, 0.25), false);

	QCOMPARE(clt(1000, 2000, 0.25), true);
	QCOMPARE(clt(1000, 1001, 0.25), false);
}

void test_functions::testCgt()
{
	QCOMPARE(cgt(0.0, 0.0, 0.25), false);
	QCOMPARE(cgt(0.0, -0.125, 0.25), false);
	QCOMPARE(cgt(0.0, -0.5, 0.25), true);

	QCOMPARE(cgt(-100, -1000, 0.25), true);
	QCOMPARE(cgt(-999, -1000, 0.25), false);

	QCOMPARE(cgt(2000, 1000, 0.25), true);
	QCOMPARE(cgt(1001, 1000, 0.25), false);
}

}

QTEST_MAIN(cutehmi::test_functions)
#include "test_functions.moc"

//(c)C: Copyright © 2019-2020, Michał Policht <michal@policht.pl>, Yuri Chornoivan <yurchor@ukr.net>. All rights reserved.
//(c)C: This file is a part of CuteHMI.
//(c)C: CuteHMI is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//(c)C: CuteHMI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//(c)C: You should have received a copy of the GNU Lesser General Public License along with CuteHMI.  If not, see <https://www.gnu.org/licenses/>.
