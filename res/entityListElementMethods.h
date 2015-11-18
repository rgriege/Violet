#include "violet/Engine.h"

class Violet::Engine;

DEFINE_EXTERNAL_METHOD(DescribeMethod, void (const Violet::Engine & engine));
DEFINE_EXTERNAL_METHOD(ExpandMethod, void ());
DEFINE_EXTERNAL_METHOD(ShrinkMethod, void ());
