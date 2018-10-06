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
        inline bool operator >(const Tag& t) const {
            return first > t.first;
        }
    };

    struct DreamcastScan {
        const int Width;
        constexpr DreamcastScan(int _w) :Width(_w) {}
        struct Iterator {
            int w; // width
            int i, o; // index, offset
            constexpr Iterator(int _w, int _i, int _o) :w(_w), i(_i), o(_o) {}

            inline pair<int, int> operator*() const {
                if (i < w - 1) return make_pair(o + i, o);
                else if (i < (w - 1) * 2) return make_pair(o + w - 1, o + i - (w - 1));
                else if (i < (w - 1) * 3) return make_pair(o + (w - 1) * 3 - i, o + w - 1);
                else return make_pair(o, o + (w - 1) * 4 - i);
            }
            inline Iterator& operator++() {
                if (++i >= (w - 1) * 4) i = 0, w -= 2, o += 1;
                return *this;
            }
            inline bool operator!=(const Iterator& it) const {
                return w != it.w || i != it.i;
            }
        };
        constexpr inline Iterator begin() const { return Iterator(Width, 0, 0); }
        constexpr inline Iterator end() const { return Iterator(-(Width & 1), 0, 0); }
    };

//------------------------------------------------------------------------------
    namespace util {

        // @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        //using ITER = vector<Piece>::const_iterator;
        ITER isIntersectPieces(ITER begin, ITER end, const Piece& piece) {
            auto it = begin
            for (; it != end; ++it) {
                const Piece& p = *it;
                if (Util::IsIntersect(
                        piece.pos(), piece.width(), piece.height(),
                        p.pos(), p.width(), p.height()
                    )) {
                    return it;
                }
            }
            return it;
        }

        inline bool isIntersectPieces(const Piece& p1, const Piece& p2) {
            return Util::IsIntersect(
                p1.pos(), p1.width(), p1.height(),
                p2.pos(), p2.width(), p2.height()
            );
        }
    }
    namespace algo {

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


        // 計算量はO*(N!)
        History solvePlacementPiece(const Oven& oven, const vector<Piece>& pieces) {
            
            vector<Tag<int, Piece*>> shuffler;
            shuffler.reserve(pieces.size());
            repeat(i, pieces.size()) shuffler.emplace_back(i, &pieces[i]);

            int bestScore = 0;
            History best;

            do {
                vector<Piece> placed(ALL(oven.bakingPieces()));

                int currScore = 0;

                // 今置こうとしている
                auto picked = shuffler.begin();
                // 直前の検証で重なった
                auto currIsec = placed.end();

                History result;

                for (auto xy : DreamcastScan(oven.width())) {
                    int x = xy.first, y = xy.second;

                    if (currIsec != placed.end()) {
                        if (util::isIntersectPieces(*currIsec, *(picked->second)))
                            continue;
                        else
                            currIsec = placed.end();
                    }

                    auto isec = util::isIntersectPieces(ALL(placed), *(picked->second));

                    if (isec != placed.end()) {
                        currIsec = isec;
                    }
                    else {
                        placed.push_back(*(picked->second));
                        currScore += picked->second->score;
                        result.emplace_back(picked->first, Vector2i(x, y));
                        ++picked;
                    }
                }
                if (bestScore < currScore) {
                    bestScore = currScore;
                    best = move(result);
                }

            } while (next_permutation(ALL(shuffler)));
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

    vector<Piece> bakingLargePieces;
    for (auto& p : oven.bakingPieces())
        if (p.height() * p.width() >= 20) bakingLargePieces.push_back(p);
    

    list<Tag<pair<int, int>, pair<int, Piece>>> pieces;
    //repeat(i, laneS.pieces().count())
    //    pieces.emplace_back(make_pair(laneS.pieces()[i].height(), laneS.pieces()[i].width()), make_pair(i + 8, laneS.pieces()[i]));
    repeat(i, laneL.pieces().count())
        pieces.emplace_back(make_pair(laneL.pieces()[i].height(), laneL.pieces()[i].width()), make_pair(i, laneL.pieces()[i]));
    pieces.sort(greater<decltype(pieces)::value_type>());

    Tag<int, Piece> largeBest(0, Piece());
    while (true) {
        auto mark = pieces.end();
        for (auto it = pieces.begin(); it != pieces.end(); ++it) {
            auto& p = *it;
            const auto& piece = p.second.second;

            auto i = p.second.first;
            for (auto xy : DreamcastScan(oven.width())) {
                int x = xy.first, y = xy.second;
                if (util::isIntersectPieces(ALL(bakingLargePieces), piece) != bakingLargePieces.end()) {
                    if (largeBest.first < piece.score()) {
                        largeBest = decltype(largeBest)(piece.score(), piece);
                        mark = it;
                    }
                }
                if (oven.isAbleToPut(piece, Vector2i(x, y))) {
                    return Action::Put(CandidateLaneType_Large, i, Vector2i(x, y));
                }
            }
        }

        if (largeBest.first > 0) {
            bakingLargePieces.push_back(largeBest.second);
            largeBest = decltype(largeBest)(0, Piece());
            pieces.erase(mark);
        }
        else
            break;
    }

    pieces.clear();
    repeat(i, laneS.pieces().count())
        pieces.emplace_back(make_pair(laneS.pieces()[i].height(), laneS.pieces()[i].width()), make_pair(i, laneS.pieces()[i]));
    pieces.sort(greater<decltype(pieces)::value_type>());


    for (auto p : pieces) {
        const auto& piece = p.second.second;
        auto i = p.second.first;
        for (auto xy : DreamcastScan(oven.width())) {
            int x, y;
            x = xy.first;
            y = xy.second;
            if (oven.isAbleToPut(piece, Vector2i(x, y))) {
                return Action::Put(CandidateLaneType_Small, i, Vector2i(x, y));
            }
        }
    }
    return Action::Wait();
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
