﻿//------------------------------------------------------------------------------
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

#include <chrono>
#include <iomanip>

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


    class DreamcastScan {
        const int Height, Width;
    public:
        constexpr DreamcastScan(int _h, int _w) :Height(_h), Width(_w) {}
        struct Iterator {
            int h, w; // width
            int i, o; // index, offset
            constexpr Iterator(int _h, int _w, int _i, int _o) :h(_h), w(_w), i(_i), o(_o) {}

            inline pair<int, int> operator*() const {
                if (i < w - 1) return make_pair(o, o + i);
                else if (i < (w - 1) + (h - 1)) return make_pair(o + i - (w - 1), o + w - 1);
                else if (i < (w - 1) * 2 + (h - 1)) return make_pair(o + h - 1, o + (w - 1) * 2 + (h - 1) - i);
                else return make_pair(o + (w - 1) * 2 + (h - 1) * 2 - i, o);
            }
            inline Iterator& operator++() {
                if (++i >= (w - 1) * 2 + (h - 1) * 2) {
                    i = 0, w -= 2, h -= 2, o += 1;
                    if (w <= 0 || h <= 0) w = h = 0;
                }
                if (w <= 0 || h <= 0 || ((w == 1 || h == 1) && i >= w + h - 1)) i = w = h = 0;
                return *this;
            }
            inline bool operator!=(const Iterator& another) const {
                return w != another.w || h != another.h || i != another.i;

            }
        };
        constexpr inline Iterator begin() const { return Iterator(Height, Width, 0, 0); }
        constexpr inline Iterator end() const { return Iterator(0, 0, 0, 0); }
    };


    class WipeScan {
        const int Height, Width;
        const bool Tr;
    public:
        constexpr WipeScan(int _h, int _w, bool _tr = false) :Height(_h), Width(_w), Tr(_tr) {}
        struct Iterator {
            const int h, w; // width
            int i, j; // index
            const bool tr;
            constexpr Iterator(int _h, int _w, int _i, int _j, bool _tr) :h(_h), w(_w), i(_i), j(_j), tr(_tr) {}

            inline pair<int, int> operator*() const {
                return make_pair(i & 1 ? h - 1 - i / 2 : i / 2, j & 1 ? w - 1 - j / 2 : j / 2);
            }
            inline Iterator& operator++() {
                if (!tr && ++j >= w) j = 0, ++i;
                if (tr && ++i >= h) i = 0, ++j;
                return *this;
            }
            inline bool operator!=(const Iterator& another) const {
                return w != another.w || i != another.i || j != another.j;

            }
        };
        constexpr inline Iterator begin() const { return Iterator(Height, Width, 0, 0, Tr); }
        constexpr inline Iterator end() const { return Iterator(Height, Width, Tr ? 0 : Height, Tr ? Width : 0, Tr); }
    };

    template<typename ITER>
    class IteratorWithIndex {
        //using ITER = int*;
        const ITER begin_;
        const ITER end_;
    public:
        IteratorWithIndex(ITER _begin, ITER _end) :begin_(move(_begin)), end_(move(_end)) { }
        struct Iterator {
            ITER it;
            int i;
            Iterator(ITER _it, int _i) :it(move(_it)), i(_i) { }
            inline pair<int, decltype(*it)> operator*() const { return pair<int, decltype(*it)>(i, *it); }
            inline Iterator& operator++() { ++it, ++i; return *this; }
            inline auto operator!=(const Iterator& another) const -> decltype(it != another.it) { return it != another.it; }
        };
        inline Iterator begin() const { return Iterator(begin_, 0); }
        inline Iterator end() const { return Iterator(end_, 0); }
    };
    template<typename ITER> inline IteratorWithIndex<ITER> make_IteratorWithIndex(ITER _begin, ITER _end) { 
        return IteratorWithIndex<ITER>(_begin, _end);
    }

//------------------------------------------------------------------------------
    namespace util {
        static mt19937_64 randdev(8901016);

        // 
        template<typename T> inline T rand(T l, T h) { return uniform_int_distribution<T>(l, h)(randdev); }
        template<> inline double rand<double>(double l, double h) { return uniform_real_distribution<double>(l, h)(util::randdev); }
        template<> inline float rand<float>(float l, float h) { return uniform_real_distribution<float>(l, h)(util::randdev); }


        // @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        inline ITER isIntersectPieces(ITER begin, ITER end, const Piece& piece) {
            return isIntersectPieces(begin, end, piece.pos(), piece);
        }

        // @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        //using ITER = vector<Piece>::const_iterator;
        ITER isIntersectPieces(ITER begin, ITER end, const Vector2i& pos, const Piece& piece) {
            auto it = begin;
            for (; it != end; ++it) {
                const Piece& p = *it;
                if (Util::IsIntersect(
                    pos, piece.width(), piece.height(),
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
        inline bool isIntersectPieces(const Vector2i& v1, const Piece& p1, const Vector2i& v2, const Piece& p2) {
            return Util::IsIntersect(
                v1, p1.width(), p1.height(),
                v2, p2.width(), p2.height()
            );
        }
        inline bool isInArea(int areaWidth, int areaHeight, int x, int y, int width, int height) {
            return 0 <= x && x + width <= areaWidth && 0 <= y && y + height <= areaHeight;
        }
        inline bool isInArea(const Oven& oven, int x, int y, int width, int height) {
            return 0 <= x && x + width <= oven.width() && 0 <= y && y + height <= oven.height();
        }
        inline bool isInArea(const Oven& oven, const Vector2i& pos, const Piece& piece) {
            return 0 <= pos.x && pos.x + piece.width() <= oven.width() && 0 <= pos.y && pos.y + piece.height() <= oven.height();
        }
    }

//------------------------------------------------------------------------------

    namespace mem { // TODO: なんとかする
        static int ScanMode = 0;
    }

//------------------------------------------------------------------------------
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


        History solvePlacementPiece(const int width, const int height, const vector<Piece>& placedPieces, const vector<Piece>& pieces) {
            
            vector<Tag<int, const Piece*>> shuffler;
            shuffler.reserve(pieces.size());
            for (auto p : make_IteratorWithIndex(ALL(pieces)))
                shuffler.emplace_back(p.first, &p.second);

            int bestScore = 0;
            History best;

            sort(ALL(shuffler));

            vector<Piece> placed;
            repeat(_, 500) {
            //do{
                shuffle(ALL(shuffler), util::randdev);
                // list<Piece> placed(ALL(placedPieces));
                placed = placedPieces;
                //placed.reserve(placedPieces.size() + pieces.size());

                int currScore = 0;
                History result;

                for (auto picked : shuffler) {

                    // 今置こうとしている
                    auto& piece = *picked.second;
                    // 直前の検証で重なった
                    auto currIsec = placed.end();

                    for (auto yx : WipeScan(height + 1 - piece.height(), width + 1 - piece.width(), mem::ScanMode)) {
                        Vector2i vec(yx.second, yx.first);
                        //int x = yx.first, y = yx.second; // swap

                        if (!util::isInArea(height, width, vec.x, vec.y, piece.width(), piece.height()))
                            continue;

                        if (currIsec != placed.end()) {
                            if (util::isIntersectPieces(currIsec->pos(), *currIsec, vec, piece))
                                continue;
                        }

                        auto isec = util::isIntersectPieces(ALL(placed), vec, piece);

                        if (isec == placed.end()) {
                            // 置く．
                            placed.emplace_back(vec, piece.width(), piece.height(), 114, 514);
                            currScore += piece.score();
                            result.emplace_back(picked.first, vec);
                            break;
                        }
                        else {
                            currIsec = move(isec);
                        }
                    }
                }
                if (bestScore < currScore) {
                    bestScore = currScore;
                    best = move(result);
                }

            } //while (next_permutation(ALL(shuffler)));

            return best;
        }


        // void expandPiecesRandom(vector<Piece>& placedPieces) {
        // }
    }

//------------------------------------------------------------------------------

    namespace mem {
        static algo::History myLargeCommandQueue;
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

decltype(chrono::system_clock::now()) bmLastTime;
//------------------------------------------------------------------------------
/// 各ステージ開始時に呼び出される処理。
/// @detail 各ステージに対する初期化処理が必要ならここに書きます。
/// @param aStage 現在のステージ。
void Answer::init(const Stage& aStage)
{
    clog << "ready\n";
    mem::myLargeCommandQueue.clear();
    bmLastTime = chrono::system_clock::now();
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

    mem::ScanMode = false; // TODO: 上手いこと実装
    {
        int hLong = 0, vLong = 0;
        for (auto& p : laneS.pieces()) hLong += p.width(), vLong += p.height();
        for (auto& p : laneL.pieces()) hLong += p.width(), vLong += p.height();
        if (hLong *3 < vLong *2) mem::ScanMode = true;
    }

    vector<Piece> bakingPieces(ALL(oven.bakingPieces()));
    vector<Piece> bakingUnignorablePieces;
    for (auto& p : bakingPieces) {
        if (p.restRequiredHeatTurnCount() > 10) bakingUnignorablePieces.push_back(p);
        //if (p.height() * p.width() >= 10) bakingUnignorablePieces.push_back(p); // 
    }
    

    vector<Piece> laneLPieces(ALL(laneL.pieces()));
    vector<Piece> laneSPieces(ALL(laneS.pieces()));

    // list<Tag<pair<int, int>, pair<int, Piece>>> pieces;
    // //repeat(i, laneS.pieces().count())
    // //    pieces.emplace_back(make_pair(laneS.pieces()[i].height(), laneS.pieces()[i].width()), make_pair(i + 8, laneS.pieces()[i]));
    // repeat(i, laneL.pieces().count())
    //     pieces.emplace_back(make_pair(laneL.pieces()[i].height(), laneL.pieces()[i].width()), make_pair(i, laneL.pieces()[i]));
    // pieces.sort(greater<decltype(pieces)::value_type>());

    if (mem::myLargeCommandQueue.empty()) {
        auto placements = algo::solvePlacementPiece(oven.width(), oven.height(), bakingUnignorablePieces, laneLPieces);

        placements.sort([&laneLPieces](const algo::History::value_type& p1, const algo::History::value_type& p2) {
            return laneLPieces[p1.first].requiredHeatTurnCount() > laneLPieces[p2.first].requiredHeatTurnCount();
        });
        mem::myLargeCommandQueue = move(placements);
    }

    // if (!placements.empty()) {
    //     int hscore = 0, cscore = 0;
    //     for (auto& p : myLargeCommandQueue) hscore += laneLPieces[p.first].score();
    //     for (auto& p : placements) cscore += laneLPieces[p.first].score();
    //     if (hscore < cscore) {
    //         myLargeCommandQueue = move(placements);
    //     }
    // }
    // TODO: sort placements
    
    iterate (it, mem::myLargeCommandQueue.begin(), mem::myLargeCommandQueue.end()) {
        auto p = *it;
        auto& piece = laneLPieces[p.first];
        if (oven.isAbleToPut(piece, p.second)) {
            for (auto& q : mem::myLargeCommandQueue)
                if (q.first > p.first) --q.first;
            mem::myLargeCommandQueue.erase(it);
            return Action::Put(CandidateLaneType_Large, p.first, p.second);
        }
        // 無理やり載せる
        bakingPieces.emplace_back(p.second, piece.width(), piece.height(), piece.restRequiredHeatTurnCount(), piece.score());
    }

    int bestScore = 1e9;
    Action best = Action::Wait();

    for (auto p : make_IteratorWithIndex(ALL(laneSPieces))) {
        const auto& piece = p.second;
        auto i = p.first;
        // if (bestScore >= piece.score()) continue;
        if (bestScore <= piece.restRequiredHeatTurnCount()) continue;
        for (auto xy : WipeScan(oven.height() + 1 - piece.height(), oven.width() + 1 - piece.width(), mem::ScanMode)) {
            int x, y;
            y = xy.first;
            x = xy.second;
            //if (!vflag) swap(x, y);
            if (util::isInArea(oven, Vector2i(x, y), piece) && util::isIntersectPieces(ALL(bakingPieces), Vector2i(x, y), piece) == bakingPieces.end()) {
                bestScore = piece.restRequiredHeatTurnCount();
                best = Action::Put(CandidateLaneType_Small, i, Vector2i(x, y));
                break;
            }
        }
    }
    return best;
}

//------------------------------------------------------------------------------
/// 各ステージ終了時に呼び出される処理。
/// @detail 各ステージに対する終了処理が必要ならここに書きます。
/// @param aStage 現在のステージ。
void Answer::finalize(const Stage& aStage)
{
    clog << scientific << setprecision(2) << double((chrono::system_clock::now() - bmLastTime).count()) << endl;
}
} // namespace
// EOF
