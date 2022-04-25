#define protected public
#include "src/plugin-keyboard/window/indexview.h"
#undef protected

#include "gtest/gtest.h"

#include <iostream>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class Tst_IndexView : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    IndexView *view = nullptr;
};

void Tst_IndexView::SetUp()
{
    view = new IndexView();
}

void Tst_IndexView::TearDown()
{
    delete view;
    view = nullptr;
}

TEST_F(Tst_IndexView, IndexView)
{
//    EXPECT_NO_THROW(view->onClick("test"));
    EXPECT_NO_THROW(view->scrollContentsBy(10,10));
}
