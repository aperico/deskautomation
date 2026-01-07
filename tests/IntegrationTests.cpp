#include <gtest/gtest.h>

#include "DeskController.h"
#include "HAL.h"
#include "hal_mock/HALMock.h"
#include "PinConfig.h"

/* Test fixture that initializes HAL and application */
class IntegrationTest : public ::testing::Test {
protected:
    DeskAppInputs_t inputs;
    DeskAppOutputs_t outputs;

    void SetUp() override {
        /* initialize HAL mock and app */
        HAL_Init();
        DeskApp_task_init(&inputs, &outputs);

        /* clear inputs/outputs and hardware pins */
        inputs = { false, false, false, false };
        outputs = { false, false, false, false };

        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        analogWrite(ENA, 0);
        digitalWrite(LED_LEFT_PIN, LOW);
        digitalWrite(LED_RIGHT_PIN, LOW);
        digitalWrite(ERROR_LED, LOW);
    }
};

/* Helper to apply app outputs to HAL and run HAL periodic work */
static void apply_and_sync(const DeskAppOutputs_t &outputs) {
    HAL_ProcessAppState(APP_TASK_SUCCESS, &outputs);
    HAL_Task();
}

/* Traceability: SR-02, UC-02, Architecture: Functional / Use Case View */
TEST_F(IntegrationTest, Smoke_UpMovement_SR02_UC02) {
    inputs.btUPPressed = true;
    inputs.upperLimitActive = false;
    inputs.lowerLimitActive = false;

    ASSERT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    apply_and_sync(outputs);

    EXPECT_EQ(digitalRead(IN1), HIGH);
    EXPECT_EQ(digitalRead(IN2), LOW);
    EXPECT_GT(digitalRead(ENA), 0);
    EXPECT_EQ(digitalRead(LED_LEFT_PIN), HIGH);   // Up LED
    EXPECT_EQ(digitalRead(LED_RIGHT_PIN), LOW);
    EXPECT_EQ(digitalRead(ERROR_LED), LOW);
}

/* Traceability: SR-03, UC-03, Architecture: Functional / Use Case View */
TEST_F(IntegrationTest, Smoke_DownMovement_SR03_UC03) {
    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = false;
    inputs.upperLimitActive = false;

    ASSERT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    apply_and_sync(outputs);

    EXPECT_EQ(digitalRead(IN1), LOW);
    EXPECT_EQ(digitalRead(IN2), HIGH);
    EXPECT_GT(digitalRead(ENA), 0);
    EXPECT_EQ(digitalRead(LED_RIGHT_PIN), HIGH);  // Down LED
    EXPECT_EQ(digitalRead(LED_LEFT_PIN), LOW);
    EXPECT_EQ(digitalRead(ERROR_LED), LOW);
}

/* Traceability: SR-07, UC-07, Architecture: Safety & Error Handling */
TEST_F(IntegrationTest, Safety_SimultaneousButtons_NoMovement_SR07_UC07) {
    inputs.btUPPressed = true;
    inputs.btDOWNPressed = true;

    ASSERT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    apply_and_sync(outputs);

    EXPECT_EQ(digitalRead(IN1), LOW);
    EXPECT_EQ(digitalRead(IN2), LOW);
    EXPECT_EQ(digitalRead(ENA), 0);
    EXPECT_EQ(digitalRead(LED_LEFT_PIN), LOW);
    EXPECT_EQ(digitalRead(LED_RIGHT_PIN), LOW);
    EXPECT_EQ(digitalRead(ERROR_LED), LOW); /* simultaneous is non-fatal by design */
}

/* Traceability: SR-02, UC-02, Architecture: Motion Control / Dwell Management */
TEST_F(IntegrationTest, Limit_StopOnUpperLimitDuringMoveUp_SR02_UC02) {
    /* Step 1: start moving up */
    inputs.btUPPressed = true;
    inputs.upperLimitActive = false;
    ASSERT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    apply_and_sync(outputs);

    EXPECT_EQ(digitalRead(IN1), HIGH); /* moving up */

    /* Step 2: upper limit becomes active -> app should stop */
    inputs.btUPPressed = true;
    inputs.upperLimitActive = true;
    ASSERT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    apply_and_sync(outputs);

    EXPECT_EQ(digitalRead(ENA), 0); /* motor stopped */
    EXPECT_EQ(digitalRead(LED_LEFT_PIN), LOW);
    EXPECT_EQ(digitalRead(ERROR_LED), LOW);
}
