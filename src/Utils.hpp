/**
 * This file is part of IDASkins IDA PRO plugin (zyantific.com).
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Joel Höner (athre0z)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <QString>
#include <QDir>

namespace Utils
{

// ============================================================================================= //
// IDASkins related functions                                                                    //
// ============================================================================================= //

QString     getThemesPath();
QDir        getThemesDir();
bool        getCurrentThemeDir(QDir &dir);

// ============================================================================================= //
// [NonCopyable]                                                                                 //
// ============================================================================================= //

/**
 * @brief Makes derived classes non-copyable.
 */
class NonCopyable
{
    NonCopyable(const NonCopyable&); // not implemented
    NonCopyable& operator = (const NonCopyable&); // not implemented
public:
    NonCopyable() {}
    virtual ~NonCopyable() {}
};

// ============================================================================================= //
// [NonInstantiable]                                                                             //
// ============================================================================================= //

/**
 * @brief Makes derived classes non-instantiable.
 * A class inheriting from this class can only be instantiated by itself
 * or deriving classes.
 */
class NonInstantiable
{
protected:
    NonInstantiable() {}
};

// ============================================================================================= //
// [Singleton]                                                                                   //
// ============================================================================================= //

template<typename T>
class Singleton : public NonCopyable
{
    static T *m_instance;
protected:
    Singleton() {}
    virtual ~Singleton() {}
public:
    static T& instance();
    static void freeInstance();
    static bool isInstantiated();
};

// ============================================================================================= //
// Implementation of inline methods [Singleton]                                                  //
// ============================================================================================= //

template<typename T> T *Singleton<T>::m_instance = nullptr;

template<typename T> inline
T& Singleton<T>::instance()
{
    if (!m_instance)
        m_instance = new T;
    return *m_instance;
}

template<typename T> inline
void Singleton<T>::freeInstance()
{
    if (m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

template<typename T> inline
bool Singleton<T>::isInstantiated()
{
    return m_instance != nullptr;
}

// ============================================================================================= //

}

#endif
