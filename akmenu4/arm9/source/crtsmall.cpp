/*
    crtsmall.cpp
    Copyright (C) 2009 yellow wood goblin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <new>
#include <nds.h>

const std::nothrow_t std::nothrow={};

static void Halt(void) __attribute__((__noreturn__));

static void Halt(void)
{
  while(true) swiWaitForVBlank();
}

void* operator new(std::size_t size)
{
  void* result=malloc(size);
  if(!result) Halt();
  return result;
}

void* operator new(std::size_t size,const std::nothrow_t&)
{
  return malloc(size);
}

void* operator new[](std::size_t size)
{
  return ::operator new(size);
}

void* operator new[](std::size_t size,const std::nothrow_t&)
{
  return ::operator new(size,std::nothrow);
}

void operator delete(void* block)
{
  free(block);
}

void operator delete[] (void *ptr)
{
  ::operator delete(ptr);
}

namespace std
{
  void __throw_bad_alloc(void)
  {
    Halt();
  }
  void __throw_length_error(const char*)
  {
    Halt();
  }
  void __throw_out_of_range(const char*)
  {
    Halt();
  }
  void __throw_logic_error(const char*)
  {
    Halt();
  }
}

extern "C"
{
  void __gxx_personality_v0(void)
  {
    Halt();
  }
  void __cxa_end_catch(void)
  {
    Halt();
  }
  void __cxa_begin_catch(void)
  {
    Halt();
  }
  void __cxa_rethrow(void)
  {
    Halt();
  }
  void __cxa_end_cleanup(void)
  {
    Halt();
  }
  void __cxa_pure_virtual(void)
  {
    Halt();
  }
  void __aeabi_unwind_cpp_pr0(void)
  {
    Halt();
  }
  void __aeabi_unwind_cpp_pr1(void)
  {
    Halt();
  }
  void __aeabi_unwind_cpp_pr2(void)
  {
    Halt();
  }
}
