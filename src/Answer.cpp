//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2018 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------
#include "Answer.hpp"

#include <limits>
#include <typeinfo>
#include <initializer_list>

#include <tuple>
#include <type_traits>
#include <memory>
#include <bitset>

#include <array>
#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <iterator>
#include <iostream>

#include <algorithm>

#include <random>
#include <valarray>
#include <numeric>

#include <cmath>

using namespace std;


#define ALL(v) (v).begin(),(v).end()
// #define repeat(cnt,l) for(remove_reference<remove_const<decltype(l)>::type>::type cnt=0;(cnt)<(l);++(cnt))
#define repeat(cnt,l) for(auto cnt=0;(cnt)<(l);++(cnt))
#define rrepeat(cnt,l) for(auto cnt=(l)-1;0<=(cnt);--(cnt))
#define iterate(cnt,b,e) for(auto cnt=(b);(cnt)!=(e);++(cnt))
#define diterate(cnt,b,e) for(auto cnt=(b);(cnt)!=(e);--(cnt))


//------------------------------------------------------------------------------
namespace hpc {

    template<typename T1, typename T2>
    struct Tag {
        T1 first; T2 second;
        Tag(T1 t1, T2 t2) :first(t1), second(t2) {}
        Tag(const pair<T1, T2>& p) :first(p.first), second(p.second) { }
        Tag(const pair<T1, T2>&& p) :first(p.first), second(p.second) { }

        inline bool operator == (const Tag& t) const {
            return first == t.first;
        }
        inline bool operator <(const Tag& t) const {
            return first < t.first;
        }
    };

//------------------------------------------------------------------------------
    namespace util {

        using History = list<pair<int, Vector2i>>;

        struct StateData {
            Oven oven;
            History history;
            StateData(Oven&& o) :oven(o) {}
            StateData(Oven&& o, const decltype(history)& h)
                :oven(o), history(h) {}
        };

        using State = Tag<int, StateData>;

        // TLE
        pair<int, Vector2i> chokudaiSearch(const Stage& aStage) {
            // assert(laneS.pieces().count() == 8);

            auto& laneS = aStage.candidateLane(CandidateLaneType_Small);
            auto& laneL = aStage.candidateLane(CandidateLaneType_Large);
            vector<Piece> pieces; pieces.reserve(16);
            for (auto& p : laneS.pieces()) pieces.push_back(p);
            for (auto& p : laneL.pieces()) pieces.push_back(p);
            list<Piece> cookies;

            auto& oven = aStage.oven();
            
            priority_queue<State> beamStack[20];
            beamStack[0].emplace(0, StateData(Oven(oven)));

            int bestScore = 0;
            History bestHistory;

            repeat(_, 10) {
                repeat(depth, 16) {
                    if (beamStack[depth].empty()) continue;
                    auto currData = move(beamStack[depth].top().second);
                    int currScore = beamStack[depth].top().first;
                    beamStack[depth].pop();

                    if (currData.history.size() >= 4) continue; // 深いところまでやらない

                    auto& piece = pieces[depth];
                    const auto& currOven = currData.oven;

                    repeat(y, currOven.height() - piece.height() + 1) {
                        repeat(x, currOven.width() - piece.width() + 1) {
                            if (!currOven.isAbleToPut(piece, Vector2i(x, y))) continue;

                            auto newData = currData;
                            cookies.push_back(piece);
                            newData.oven.tryToBake(&cookies.back(), Vector2i(x, y));
                            newData.oven.bakeAndDiscard();
                            newData.history.emplace_back(int8_t(depth), Vector2i(x, y));

                            int newScore = currScore + piece.height()*piece.width();
                            if (newScore > bestScore) {
                                bestScore = newScore;
                                bestHistory = newData.history;
                            }
                            beamStack[depth + 1].emplace(newScore, move(newData));
                        }
                    }
                    currData.oven.bakeAndDiscard();
                    beamStack[depth + 1].emplace(currScore, move(currData));
                }
            }

            return bestHistory.front();
        }


        History chokudaiSearchLarge(const Stage& aStage) {
            // assert(laneS.pieces().count() == 8);

            auto& laneL = aStage.candidateLane(CandidateLaneType_Large);
            vector<Piece> pieces; pieces.reserve(16);
            for (auto& p : laneL.pieces()) pieces.push_back(p);
            list<Piece> cookies;

            auto& oven = aStage.oven();

            priority_queue<State> beamStack[20];
            beamStack[0].emplace(0, StateData(Oven(oven)));

            int bestScore = 0;
            History bestHistory;

            repeat(_, 10) {
                repeat(depth, 8) {
                    if (beamStack[depth].empty()) continue;
                    auto currData = move(beamStack[depth].top().second);
                    int currScore = beamStack[depth].top().first;
                    beamStack[depth].pop();

                    auto& piece = pieces[depth];
                    const auto& currOven = currData.oven;

                    repeat(y, currOven.height() - piece.height() + 1) {
                        repeat(x, currOven.width() - piece.width() + 1) {
                            if (!currOven.isAbleToPut(piece, Vector2i(x, y))) continue;

                            auto newData = currData;
                            cookies.push_back(piece);
                            newData.oven.tryToBake(&cookies.back(), Vector2i(x, y));
                            newData.oven.bakeAndDiscard();
                            newData.history.emplace_back(int8_t(depth), Vector2i(x, y));

                            int newScore = currScore + piece.height()*piece.width();
                            if (newScore > bestScore) {
                                bestScore = newScore;
                                bestHistory = newData.history;
                            }
                            beamStack[depth + 1].emplace(newScore, move(newData));
                        }
                    }
                    currData.oven.bakeAndDiscard();
                    beamStack[depth + 1].emplace(currScore, move(currData));
                }
            }

            return bestHistory;
        }
    }
//------------------------------------------------------------------------------
/// コンストラクタ。
/// @detail 最初のステージ開始前に実行したい処理があればここに書きます。
Answer::Answer()
{
}

//------------------------------------------------------------------------------
/// デストラクタ。
/// @detail 最後のステージ終了後に実行したい処理があればここに書きます。
Answer::~Answer()
{
}

//------------------------------------------------------------------------------
/// 各ステージ開始時に呼び出される処理。
/// @detail 各ステージに対する初期化処理が必要ならここに書きます。
/// @param aStage 現在のステージ。
void Answer::init(const Stage& aStage)
{
    clog << "ready\n";
}


static util::History moveStacked;
array<int, 8> order;

//------------------------------------------------------------------------------
/// このターンでの行動を指定する。
/// @detail 希望する行動を Action の static 関数で生成して return してください。
///         正常ではない行動を return した場合、何も起きません。
///         詳細は Action のヘッダを参照してください。
/// @param aStage 現在のステージ。
Action Answer::decideNextAction(const Stage& aStage)
{
    // clog << "act:" << aStage.turn() << '\n';
    auto& laneS = aStage.candidateLane(CandidateLaneType_Small);
    auto& laneL = aStage.candidateLane(CandidateLaneType_Large);
    auto& oven = aStage.oven();

    if ([&]() {
        repeat(i, laneL.pieces().count()) {
            repeat(y, oven.height()) {
                repeat(x, oven.width()) {
                    if (aStage.oven().isAbleToPut(laneL.pieces()[i], Vector2i(x, y))) {
                        return false;
                    }
                }
            }
        }
        repeat(i, laneL.pieces().count()) {
            repeat(y, oven.height()) {
                repeat(x, min(oven.width(), 5)) {
                    if (aStage.oven().isAbleToPut(laneS.pieces()[i], Vector2i(x, y))) {
                        return false;
                    }
                }
            }
        }
        return true;
    }()) {
        return Action::Wait();
    }

    if (moveStacked.empty()) {
        moveStacked = util::chokudaiSearchLarge(aStage);
        fill(ALL(order), 0);
    }
    if (moveStacked.empty()) return Action::Wait();

    auto act = moveStacked.front(); moveStacked.pop_front();
    int idx = act.first;
    repeat(i, act.first)
        idx += order[i];
    order[act.first] -= 1;

    return Action::Put(CandidateLaneType_Large, idx, act.second);
}

//------------------------------------------------------------------------------
/// 各ステージ終了時に呼び出される処理。
/// @detail 各ステージに対する終了処理が必要ならここに書きます。
/// @param aStage 現在のステージ。
void Answer::finalize(const Stage& aStage)
{
}

} // namespace
// EOF
