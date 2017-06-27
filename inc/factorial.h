/** Calculates the factorial of the given number.

   @file factorial.h
   @ingroup misclib

   @author Christian D&ouml;nges <cd@platypus-projects.de>

   @note The master repository for this file is at
    <a href="https://github.com/cdoenges/misclib">https://github.com/cdoenges/misclib</a>

    LICENSE

    Copyright (c) 2012-2016, 2017 Christian Doenges (Christian D&ouml;nges) All rights
    reserved.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */
#include <limits.h>


/** The largest number of which a factorial can be computed. */
extern const unsigned MAX_FACTORIAL_INPUT;


//lint -esym(714, factorial, ffactorial)
//lint -esym(759, factorial, ffactorial)

/** Calculates the factorial of the given number.

   The factorial of n is n * (n-1) * ... * 2 * 1.
   See https://en.wikipedia.org/wiki/Factorial for more information.
   
  @param n The number to calculate the factorial of.
*/
extern unsigned factorial(unsigned n);


/** Calculates the factorial of the given number.

   The factorial of n is n * (n-1) * ... * 2 * 1.
   See https://en.wikipedia.org/wiki/Factorial for more information.
   
  @param n The number to calculate the factorial of.
*/
extern double ffactorial(double n);
