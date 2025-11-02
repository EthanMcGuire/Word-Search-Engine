#pragma once

#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H

#include "parameterVariant.hpp"
#include <string>
#include <vector>

class Object;
class GameManager;

class ObjectFactory
{
    public:
        //Don't make instances of this class
		ObjectFactory()=delete;
	    ObjectFactory(const ObjectFactory&)=delete;
	    ObjectFactory(ObjectFactory&&)=delete;

        template <typename... Args>
        static Object* createObject(GameManager *gameManager, std::string objectName, double x, double y, Args... args);

        static Object* createObject(GameManager *gameManager, std::string objectName, double x, double y, std::vector<ParameterVariant> args);
};

#include "objectFactory.tpp"

#endif