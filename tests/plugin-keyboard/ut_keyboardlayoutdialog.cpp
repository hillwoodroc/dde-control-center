#include "src/plugin-keyboard/window/keyboardlayoutdialog.h"

#include <QSignalSpy>
#include <QPushButton>

#include "gtest/gtest.h"

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class Tst_KeyboardLayoutDialog : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    KeyboardLayoutDialog *dialog = nullptr;
};

void Tst_KeyboardLayoutDialog::SetUp()
{
    dialog = new KeyboardLayoutDialog();
}

void Tst_KeyboardLayoutDialog::TearDown()
{
    delete dialog;
    dialog = nullptr;
}

TEST_F(Tst_KeyboardLayoutDialog, emitSignal)
{

}
