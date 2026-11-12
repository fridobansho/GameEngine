#include "TestHarness.h"
#include <GameEngine/ScreenList.h>
#include <GameEngine/IGameScreen.h>

class MockScreen : public GameEngine::IGameScreen {
public:
    MockScreen(int index, int next, int prev)
        : m_idx(index), m_next(next), m_prev(prev) {
        m_screenIndex = index;
    }

    int getNextScreenIndex() const override { return m_next; }
    int getPreviousScreenIndex() const override { return m_prev; }
    void build() override { m_built = true; }
    void destroy() override { m_destroyed = true; }
    void onEntry() override {}
    void onExit() override {}
    void update() override {}
    void draw() override {}

    bool isBuilt() const { return m_built; }
    bool isDestroyed() const { return m_destroyed; }

private:
    int m_idx;
    int m_next;
    int m_prev;
    bool m_built = false;
    bool m_destroyed = false;
};

TEST_CASE(ScreenList_Navigation) {
    GameEngine::ScreenList screenList(nullptr);

    MockScreen* s0 = new MockScreen(0, 1, SCREEN_INDEX_NO_SCREEN);
    MockScreen* s1 = new MockScreen(1, 2, 0);
    MockScreen* s2 = new MockScreen(2, SCREEN_INDEX_NO_SCREEN, 1);

    screenList.addScreen(s0);
    screenList.addScreen(s1);
    screenList.addScreen(s2);

    // Initial state
    CHECK(screenList.getCurrent() == nullptr);
    CHECK(s0->isBuilt());
    CHECK(s1->isBuilt());
    CHECK(s2->isBuilt());

    // Set to screen 0
    screenList.setScreen(0);
    GameEngine::IGameScreen* cur = screenList.getCurrent();
    CHECK(cur == s0);

    // Advance to screen 1
    cur = screenList.moveNext();
    CHECK(cur == s1);

    // Advance to screen 2
    cur = screenList.moveNext();
    CHECK(cur == s2);

    // Back to screen 1
    cur = screenList.movePrevious();
    CHECK(cur == s1);

    screenList.destroy();
    CHECK(s0->isDestroyed());
    CHECK(s1->isDestroyed());
    CHECK(s2->isDestroyed());
}
