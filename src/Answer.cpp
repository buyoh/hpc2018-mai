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
#include <functional>

#include <tuple>
#include <type_traits>
#include <memory>
#include <bitset>

#include <array>
#include <deque>
#include <list>
#include <forward_list>
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


    struct XorShift {
        using result_type = uint64_t;
        result_type x_;
        XorShift(result_type x = 88172645463325252ull) :x_(x) {};
        static constexpr inline result_type min() noexcept { return 0ull; }
        static constexpr inline result_type max() { return numeric_limits<result_type>::max(); }
        inline result_type operator()() noexcept { x_ ^= x_ << 7; return x_ ^= x_ >> 9; }
        inline void discard(unsigned long long z) noexcept { while (z--) operator()(); }
    };
    XorShift randdev;


    class DreamcastScan {
        const int Height, Width;
    public:
        DreamcastScan(int _h, int _w) :Height(_h), Width(_w) {}
        struct Iterator {
            int h, w; // ovenWidth
            int i, o; // index, offset
            constexpr Iterator(int _h, int _w, int _i, int _o) :h(_h), w(_w), i(_i), o(_o) {}

            inline Vector2i operator*() const {
                if (i < w - 1) return Vector2i(o + i, o);
                else if (i < (w - 1) + (h - 1)) Vector2i(o + w - 1, o + i - (w - 1));
                else if (i < (w - 1) * 2 + (h - 1)) Vector2i(o + (w - 1) * 2 + (h - 1) - i, o + h - 1);
                else Vector2i(o, o + (w - 1) * 2 + (h - 1) * 2 - i);
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
        inline Iterator begin() const { return Iterator(Height, Width, 0, 0); }
        inline Iterator end() const { return Iterator(0, 0, 0, 0); }
    };


    class WipeScan {
        const int Height, Width;
        const bool Tr;
    public:
        struct Iterator {
            const int h1, w1; // h1 = Height-1, w1 = Width-1
            int i, j; // index
            const bool tr;
            Iterator(int _h, int _w, int _i, int _j, bool _tr) :h1(_h - 1), w1(_w - 1), i(_i), j(_j), tr(_tr) { }

            inline Vector2i operator*() const {
                return Vector2i(j & 1 ? w1 - (j >> 1) : (j >> 1), i & 1 ? h1 - (i >> 1) : (i >> 1));
            }
            inline Iterator& operator++() noexcept {
                !tr && ++j > w1 && (j = 0, ++i);
                tr && ++i > h1 && (i = 0, ++j);
                return *this;
            }
            inline bool operator!=(const Iterator& another) const noexcept {
                return i != another.i || j != another.j;
            }
        };
        WipeScan(int _h, int _w, bool _tr = false) :Height(_h), Width(_w), Tr(_tr) {}
        inline Iterator begin() const { return Iterator(Height, Width, 0, 0, Tr); }
        inline Iterator end() const { return Iterator(Height, Width, Tr ? 0 : Height-1, Tr ? Width-1 : 0, Tr); }
    };



    class LScan {
        const int Height, Width;
    public:
        struct Iterator {
            int mini, maxi, sw; // ovenWidth
            int i, o = 0; // index
            Iterator(int _mini, int _maxi, int _sw, int _i, int _o) :mini(_mini), maxi(_maxi), sw(_sw), i(_i), o(_o) {}

            inline Vector2i operator*() const {
                return Vector2i(i & 1 ? o + (i >> 1) : o, i & 1 ? o : o + (i >> 1));
            }
            inline Iterator& operator++() noexcept {
                ++i;
                if (i >= (mini - o) * 2 + sw) ++i;
                if (i >= (maxi - o) * 2) i = 1, ++o;
                return *this;
            }
            inline bool operator!=(const Iterator& another) const noexcept {
                return i != another.i || o != another.o;

            }
        };
        LScan(int _h, int _w) :Height(_h), Width(_w) {}
        inline Iterator begin() const { return Iterator(min(Height, Width), max(Height, Width), Height > Width, 1, 0); }
        inline Iterator end() const { return Iterator(min(Height, Width), max(Height, Width), Height > Width, 1, min(Height, Width)); }
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



    class MyPiece {
        int x_, y_, w_, h_, remTurn_, s_, id_, future_;
    public:
        MyPiece(const Piece& p, int _id = -1, int _future = 0) :
            x_(p.pos().x), y_(p.pos().y), w_(p.width()), h_(p.height()),
            remTurn_(p.restRequiredHeatTurnCount()), s_(p.score()), id_(_id), future_(_future) {}

        MyPiece(const MyPiece& p, int _id, int _future) :
            x_(p.x_), y_(p.y_), w_(p.w_), h_(p.h_),
            remTurn_(p.remTurn_), s_(p.s_), id_(_id), future_(_future) {}

        MyPiece(int _x, int _y, int _w, int _h) :
            x_(_x), y_(_y), w_(_w), h_(_h),
            remTurn_(0), s_(0), id_(-1), future_(0) {}
        inline int x() const noexcept { return x_; }
        inline int y() const noexcept { return y_; }
        inline int width() const noexcept { return w_; }
        inline int height() const noexcept { return h_; }
        inline int restRequiredHeatTurnCount() const noexcept { return remTurn_; }
        inline int score() const noexcept { return s_; }
        inline int id() const noexcept { return id_; }
        inline int future() const noexcept { return future_; }

        inline int area() const noexcept { return w_ * h_; }

        inline MyPiece& moveTo(int _x, int _y) noexcept { x_ = _x; y_ = _y; return *this; }
        inline MyPiece& setId(int _id) noexcept { id_ = _id; return *this; }

        inline MyPiece moved(int _x, int _y) const { return MyPiece(*this).moveTo(_x, _y); }
        inline Piece to_piece() const { return Piece(Vector2i(x_, y_), w_, h_, remTurn_, s_); }
    };


    class PiecesManager {
        int nextIndex_;
        vector<int> lane_[2];
        map<int, pair<int, int>> dic_;
    public:

        void init(int sizeOfLane0, int sizeOfLane1) {
            lane_[0].resize(sizeOfLane0);
            lane_[1].resize(sizeOfLane1);
            dic_.clear();

            for (int i = 0; i < sizeOfLane0; ++i)
                lane_[0][i] = i,
                dic_[i] = make_pair(0, i);
            for (int i = 0; i < sizeOfLane1; ++i)
                lane_[1][i] = i + sizeOfLane0,
                dic_[i + sizeOfLane0] = make_pair(1, i);

            nextIndex_ = sizeOfLane0 + sizeOfLane1;
        }
        PiecesManager(int sizeOfLane0, int sizeOfLane1) { init(sizeOfLane0, sizeOfLane1); }

        inline pair<int, int> id2raw(int id) const {
            return dic_.find(id)->second;
        }
        inline int raw2id(int laneid, int idx) const {
            return lane_[laneid][idx];
        }

        inline void pick(int laneid, int idx) {
            const int size = (int)lane_[laneid].size();

            dic_.erase(lane_[laneid][idx]);

            for (auto& d : dic_)
                if (d.second.first == laneid && d.second.second > idx)
                    --d.second.second;
            for (int i = idx + 1; i < size; ++i)
                lane_[laneid][i - 1] = lane_[laneid][i];

            lane_[laneid][size-1] = nextIndex_;
            dic_[nextIndex_] = make_pair(laneid, size-1);
            ++nextIndex_;
        }
    };

//------------------------------------------------------------------------------
    namespace util {
        // static mt19937_64 randdev(8901016);
        static XorShift randdev;

        // 
        template<typename T> inline T rand(T l, T h) { return uniform_int_distribution<T>(l, h)(randdev); }
        template<> inline double rand<double>(double l, double h) { return uniform_real_distribution<double>(l, h)(util::randdev); }
        template<> inline float rand<float>(float l, float h) { return uniform_real_distribution<float>(l, h)(util::randdev); }


        inline bool isIntersect(
            const Vector2i& aLhsPos, int aLhsWidth, int aLhsHeight,
            const Vector2i& aRhsPos,int aRhsWidth, int aRhsHeight) noexcept {
            return 
                aLhsPos.x < aRhsPos.x + aRhsWidth &&
                aLhsPos.y < aRhsPos.y + aRhsHeight &&
                aRhsPos.x < aLhsPos.x + aLhsWidth &&
                aRhsPos.y < aLhsPos.y + aLhsHeight;
        }
        inline bool isIntersect(
            int aLhsPos_x, int aLhsPos_y, int aLhsWidth, int aLhsHeight,
            int aRhsPos_x, int aRhsPos_y, int aRhsWidth, int aRhsHeight) noexcept {
            return 
                aLhsPos_x < aRhsPos_x + aRhsWidth &&
                aLhsPos_y < aRhsPos_y + aRhsHeight &&
                aRhsPos_x < aLhsPos_x + aLhsWidth &&
                aRhsPos_y < aLhsPos_y + aLhsHeight;
        }

        /// @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        inline ITER isIntersectPieces(ITER begin, ITER end, const Piece& piece) {
            return isIntersectPieces(begin, end, piece.pos(), piece);
        }

        /// @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        //using ITER = vector<Piece>::const_iterator;
        inline ITER isIntersectPieces(ITER begin, ITER end, int x, int y, int width, int height) {
            for (; begin != end; ++begin) {
                if (util::isIntersect(
                    x, y, width, height,
                    begin->x(), begin->y(), begin->width(), begin->height()
                )) {
                    return move(begin);
                }
            }
            return move(begin);
        }

        /// @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        //using ITER = vector<Piece>::const_iterator;
        inline ITER isIntersectPieces(ITER begin, ITER end, const Vector2i& pos, const MyPiece& piece) {
            return isIntersectPieces(begin, end, pos.x, pos.y, piece.width(), piece.height());
        }

        inline bool isIntersectPieces(const MyPiece& p1, const MyPiece& p2) {
            return util::isIntersect(
                p1.x(), p1.y(), p1.width(), p1.height(),
                p2.x(), p2.y(), p2.width(), p2.height()
            );
        }
        inline bool isIntersectPieces(int x1, int y1, const MyPiece& p1, int x2, int y2, const MyPiece& p2) {
            return util::isIntersect(
                x1, y1, p1.width(), p1.height(),
                x2, y2, p2.width(), p2.height()
            );
        }

        inline bool isInArea(int areaWidth, int areaHeight, int x, int y, int width, int height) {
            return 0 <= x && x + width <= areaWidth && 0 <= y && y + height <= areaHeight;
        }
        inline bool isInArea(const Oven& oven, int x, int y, int width, int height) {
            return 0 <= x && x + width <= oven.width() && 0 <= y && y + height <= oven.height();
        }
        inline bool isInArea(const Oven& oven, const Vector2i& pos, const MyPiece& piece) {
            return 0 <= pos.x && pos.x + piece.width() <= oven.width() && 0 <= pos.y && pos.y + piece.height() <= oven.height();
        }


        /// @return 重なるPieceのうち，最大のrestRequiredHeatTurnCount
        template<typename ITER>
        //using ITER = vector<MyPiece>::const_iterator;
        inline int calcRestTimeIntersectPieces(ITER begin, ITER end, int x, int y, int width, int height) {
            int waitTime = 0;
            for (; begin != end; ++begin) {
                if (begin->future() == 0 &&
                    util::isIntersect(
                    x, y, width, height,
                    begin->x(), begin->y(), begin->width(), begin->height()
                )) {
                    waitTime = max(waitTime, begin->restRequiredHeatTurnCount());
                }
            }
            return waitTime;
        }


        /// @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        //using ITER = vector<Piece>::const_iterator;
        inline ITER isIntersectPiecesWithRestTime(ITER begin, ITER end, int x, int y, int width, int height, int rest) {
            for (; begin != end; ++begin) {
                if (begin->future() <= rest && util::isIntersect(
                    x, y, width, height,
                    begin->x(), begin->y(), begin->width(), begin->height()
                )) {
                    return move(begin);
                }
            }
            return move(begin);
        }
        /// @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        //using ITER = vector<Piece>::const_iterator;
        inline ITER isIntersectPiecesWithRestTime(ITER begin, ITER end, const Vector2i& pos, const MyPiece& piece) {
            return isIntersectPiecesWithRestTime(begin, end, pos.x, pos.y, piece.width(), piece.height(), piece.restRequiredHeatTurnCount());
        }
    }

//------------------------------------------------------------------------------

    namespace mem { // TODO: なんとかする
        static int ScanMode = 0;
    }

    namespace analysis {
        map<vector<int>, int> counter;
    }

//------------------------------------------------------------------------------
    namespace algo {

        using History = vector<pair<int, Vector2i>>;



        Vector2i findPlaceablePosition(const MyPiece& piece, const int areaWidth, const int areaHeight,
            const vector<MyPiece>& placedPieces, const int scanmode) {

            // 直前の検証で重なった
            auto currIsec = placedPieces.end();

            for (auto vec :
                WipeScan(areaHeight, areaWidth, scanmode)) {

                if (currIsec != placedPieces.end() &&
                    util::isIntersectPieces(currIsec->x(), currIsec->y(), *currIsec, vec.x, vec.y, piece))
                    continue;

                currIsec = util::isIntersectPiecesWithRestTime(ALL(placedPieces), vec, piece);

                if (currIsec == placedPieces.end()) {
                    return vec;
                }
            }
            return Vector2i(-1, -1);
        }
        Vector2i findPlaceablePositionReverse(const MyPiece& piece, const int areaWidth, const int areaHeight,
            const vector<MyPiece>& placedPieces, const int scanmode) {

            // 直前の検証で重なった
            auto currIsec = placedPieces.end();

            for (auto vec :
                WipeScan(areaHeight, areaWidth, scanmode)) {
                vec.x = areaWidth - 1 - vec.x;
                vec.y = areaHeight - 1 - vec.y;

                if (currIsec != placedPieces.end() &&
                    util::isIntersectPieces(currIsec->x(), currIsec->y(), *currIsec, vec.x, vec.y, piece))
                    continue;

                currIsec = util::isIntersectPiecesWithRestTime(ALL(placedPieces), vec, piece);

                if (currIsec == placedPieces.end()) {
                    return vec;
                }
            }
            return Vector2i(-1, -1);
        }


        History solvePlacementPiece2(const int ovenWidth, const int ovenHeight, const vector<MyPiece>& placedPieces, const vector<MyPiece>& pieces, int maxLoopcount, const int timeLeft) {

            // スコア計算用の関数
            auto lambdaBaseScore = [](const MyPiece& piece) {return piece.area();/*piece.score();*/ };

            vector<const MyPiece*> shuffler;
            shuffler.reserve(pieces.size());
            for (auto& p : pieces)
                shuffler.emplace_back(&p);

            //vector<const MyPiece*> sortedPieces = shuffler;
            //sort(ALL(sortedPieces), [&lambdaBaseScore](const MyPiece* lp, const MyPiece* rp) {return lambdaBaseScore(*lp) > lambdaBaseScore(*rp); });
            
            int bestScore = 0;
            History best, currResult;
            int bestFixTime = 0;

            vector<MyPiece> placed;
            repeat(loopcnt, maxLoopcount) {
                shuffle(ALL(shuffler), util::randdev);
                placed = placedPieces;

                int currScore = 0;
                currResult.clear();

                for (auto picked : shuffler) {
                    // 今置こうとしている
                    auto& piece = *picked;
                    if (piece.restRequiredHeatTurnCount() > timeLeft) continue;

                    auto vec = findPlaceablePositionReverse(piece, ovenWidth + 1 - piece.width(), ovenHeight + 1 - piece.height(),
                        placed, mem::ScanMode ^ !(randdev() & 31));

                    if (vec.x != -1) {
                        placed.emplace_back(piece.moved(vec.x, vec.y));
                        currScore += lambdaBaseScore(piece);
                        currResult.emplace_back(piece.id(), vec);
                    }

                }
                // 追加で置けるまで置く（より多く置けたなら高得点）
                repeat(extraChance, 30) {
                    // 今置こうとしている
                    auto& piece = pieces[extraChance %(pieces.size())];
                    if (piece.restRequiredHeatTurnCount() > timeLeft) continue;

                    auto vec = findPlaceablePositionReverse(piece, ovenWidth + 1 - piece.width(), ovenHeight + 1 - piece.height(),
                        placed, mem::ScanMode ^ !(randdev() & 31));

                    if (vec.x != -1) {
                        placed.emplace_back(piece.moved(vec.x, vec.y));
                        currScore += lambdaBaseScore(piece);
                    }
                }


                if (bestScore < currScore) {
                    bestScore = currScore;
                    best = move(currResult);
                    if (best.size() == shuffler.size()) break; // best
                    bestFixTime = 0;
                }
                else {
                    if (++bestFixTime > 30) break;
                }

            }

            analysis::counter[vector<int>{(int)best.size()}]++;
            return best;
        }


        History solvePlacementPiece(const int ovenWidth, const int ovenHeight,
            const vector<MyPiece>& placedPieces, const vector<MyPiece>& pieces, const int timeLeft) {

            vector<const MyPiece*> shuffler;
            shuffler.reserve(pieces.size());
            for (auto& p : pieces)
                shuffler.emplace_back(&p);

            int bestScore = 0;
            History best;

            // スコア計算用の関数
            auto lambdaBaseScore = [](const MyPiece& piece) {return piece.area();/*piece.score();*/ };

            sort(ALL(shuffler), [&lambdaBaseScore](const decltype(shuffler)::value_type& l, const decltype(shuffler)::value_type& r) {
                return lambdaBaseScore(*l) > lambdaBaseScore(*r);
            });

            function<void(vector<int>& indices, vector<MyPiece>& placed, History& currResult, int currScore)> dfs =
                [&](vector<int>& indices, vector<MyPiece>& placed, History& currResult, int currScore) -> void {

                if (bestScore < currScore) {
                    bestScore = currScore;
                    best = currResult;
                }


                bool empty = true;
                repeat(i, (int)indices.size()) {
                    int idx = indices[i];
                    if (idx < 0) continue;
                    empty = false;

                    // 今置こうとしている
                    auto& piece = *shuffler[idx];

                    if (piece.restRequiredHeatTurnCount() > timeLeft) continue;

                    // 置く場所
                    auto vec = findPlaceablePosition(piece, ovenWidth + 1 - piece.width(), ovenHeight + 1 - piece.height(),
                        placed, mem::ScanMode ^ !(randdev() & 31));

                    if (vec.x != -1) {
                        // スコア計算
                        int score = lambdaBaseScore(piece);
                        if (piece.restRequiredHeatTurnCount() > 80) {
                            if (!(vec.x == 0) &&
                                !(vec.y == ovenHeight - piece.height()) &&
                                !(vec.y == 0) &&
                                !(vec.x == ovenWidth - piece.width())) {
                                score = -1;
                            }
                        }

                        // 置く．
                        placed.emplace_back(piece.moved(vec.x, vec.y));
                        currScore += score;
                        currResult.emplace_back(piece.id(), vec);
                        indices[i] = -1;

                        dfs(indices, placed, currResult, currScore);

                        indices[i] = idx;
                        currResult.pop_back();
                        currScore -= score;
                        placed.pop_back();
                    }

                }

                if (empty) {
                    // extra chance !!

                    auto ex_placed = placed;
                    int ex_currScore = currScore;

                    repeat(extraChance, 25) {
                        // 今置こうとしている
                        auto& piece = *shuffler[extraChance % shuffler.size()];

                        if (piece.restRequiredHeatTurnCount() > timeLeft) continue;

                        // 置く場所
                        auto vec = findPlaceablePosition(piece, ovenWidth + 1 - piece.width(), ovenHeight + 1 - piece.height(),
                            placed, mem::ScanMode);

                        if (vec.x != -1) {
                            ex_placed.emplace_back(piece.moved(vec.x, vec.y));
                            ex_currScore += lambdaBaseScore(piece);
                        }
                    }
                    if (bestScore < ex_currScore) {
                        bestScore = ex_currScore;
                        best = currResult;
                    }
                }
            };

            vector<MyPiece> placed = placedPieces;
            vector<int> indices(shuffler.size()); iota(ALL(indices), 0);
            History currResult;
            dfs(indices, placed, currResult, 0);


            return best;
        }


        void generateListOfEmptyCell() {

        }


    }

//------------------------------------------------------------------------------

    namespace mem {
        static PiecesManager piecesManager(8,8);
        static algo::History myLargeCommandQueue;
        static algo::History mySmallCommandQueue;
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
    mem::mySmallCommandQueue.clear();
    bmLastTime = chrono::system_clock::now();
    mem::piecesManager.init(
        aStage.candidateLane(CandidateLaneType_Small).pieces().count(),
        aStage.candidateLane(CandidateLaneType_Large).pieces().count()
    );
}

//------------------------------------------------------------------------------
/// 各ステージ終了時に呼び出される処理。
/// @detail 各ステージに対する終了処理が必要ならここに書きます。
/// @param aStage 現在のステージ。
void Answer::finalize(const Stage& aStage)
{
    clog << "counter:\n";
    for (auto p : analysis::counter) {
        clog << '[';
        for (auto v : p.first)
            clog << ' ' << v << ' ';
        clog << "]:" << p.second << '\n';
    }
    clog << "time\n";
    clog << scientific << setprecision(2) << double((chrono::system_clock::now() - bmLastTime).count()) << '\n';

    clog << endl;
}


//------------------------------------------------------------------------------
Action onDecideAction(Action action) {
    if (!action.isWaiting())
        mem::piecesManager.pick(action.candidateLaneType() == CandidateLaneType_Large, action.pieceIndex());
    return move(action);
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
    const CandidateLaneType switchLaneType[2] = { CandidateLaneType_Small, CandidateLaneType_Large };
    auto& laneS = aStage.candidateLane(CandidateLaneType_Small);
    auto& laneL = aStage.candidateLane(CandidateLaneType_Large);
    auto& oven = aStage.oven();
    const int ovenHeight = oven.height();
    const int ovenWidth = oven.width();
    const int timeLeft = Parameter::GameTurnLimit - aStage.turn() - 1;


    laneS.recipe().maxSampleEdgeLength();

    if (/*aStage.turn() == 0*/ true) {
        //mem::ScanMode = 0; // TODO: 上手いこと実装
        int hLong = 0, vLong = 0;
        for (auto& p : laneS.pieces()) hLong += p.width(), vLong += p.height();
        for (auto& p : laneL.pieces()) hLong += p.width(), vLong += p.height();
        if (hLong * 3 < vLong * 2) mem::ScanMode = 1;
        if (vLong * 3 < hLong * 2) mem::ScanMode = 0;
    }

    vector<MyPiece> bakingPieces(ALL(oven.bakingPieces()));
    vector<MyPiece> bakingUnignorablePieces;
    for (auto& p : bakingPieces) {
        if (p.restRequiredHeatTurnCount() + (aStage.turn()%4) >= 8) bakingUnignorablePieces.push_back(p);
        //if (p.ovenHeight() * p.ovenWidth() >= 10) bakingUnignorablePieces.push_back(p); // 
    }
    

    vector<MyPiece> laneSPieces, laneLPieces;
    {
        auto& sp = laneS.pieces();
        laneSPieces.reserve(sp.count());
        repeat(i, sp.count()) {
            laneSPieces.emplace_back(sp[i], mem::piecesManager.raw2id(0, i));
        }
    }
    {
        auto& lp = laneL.pieces();
        laneLPieces.reserve(lp.count());
        repeat(i, lp.count()) {
            int id = mem::piecesManager.raw2id(1, i);
            laneLPieces.emplace_back(lp[i], id);
        }
    }


    auto solveLanePieces = [&laneSPieces, &laneLPieces](int idx) -> MyPiece&{
        auto ptr = mem::piecesManager.id2raw(idx);
        return ptr.first == 0 ? laneSPieces[ptr.second] : laneLPieces[ptr.second]; };


    auto calcScoreOfHistory = [&solveLanePieces](const algo::History& hh) -> int {
        return accumulate(ALL(hh), 0, [&solveLanePieces](int prev, algo::History::value_type p) {
            return prev + solveLanePieces(p.first).restRequiredHeatTurnCount(); });
    };



    if (mem::myLargeCommandQueue.empty()) {

        auto placements = algo::solvePlacementPiece(ovenWidth, ovenHeight, bakingUnignorablePieces, laneLPieces, timeLeft);

        if (!placements.empty()) {

            sort(ALL(placements), [&](const algo::History::value_type& p1, const algo::History::value_type& p2) {
                return solveLanePieces(p1.first).restRequiredHeatTurnCount() > solveLanePieces(p2.first).restRequiredHeatTurnCount();
            });

            mem::mySmallCommandQueue.clear();
            mem::myLargeCommandQueue = move(placements);
        }
    }
    else if (aStage.turn() > 20 && (aStage.turn() % 4 == 0)) {
        auto placements = algo::solvePlacementPiece(ovenWidth, ovenHeight, bakingUnignorablePieces, laneLPieces, timeLeft);
        
        if (calcScoreOfHistory(mem::myLargeCommandQueue) < calcScoreOfHistory(placements)) {

            sort(ALL(placements), [&](const algo::History::value_type& p1, const algo::History::value_type& p2) {
                return solveLanePieces(p1.first).restRequiredHeatTurnCount() > solveLanePieces(p2.first).restRequiredHeatTurnCount();
            });

            mem::mySmallCommandQueue.clear();
            mem::myLargeCommandQueue = move(placements);
        }
    }
    
    iterate (it, mem::myLargeCommandQueue.begin(), mem::myLargeCommandQueue.end()) {
        auto p = *it;
        auto& piece = solveLanePieces(p.first);
        if (timeLeft < piece.restRequiredHeatTurnCount()) {
            continue;
        }
        if (oven.isAbleToPut(piece.to_piece(), p.second)) {
            mem::myLargeCommandQueue.erase(it);

            auto target = mem::piecesManager.id2raw(piece.id());
            return onDecideAction(Action::Put(switchLaneType[target.first], target.second, p.second));
        }
        // 無理やり載せる
        bakingPieces.emplace_back(piece.moved(p.second.x, p.second.y),piece.id(),
            util::calcRestTimeIntersectPieces(ALL(bakingPieces), p.second.x, p.second.y, piece.width(), piece.height()));
    }


    // {
    //     int bestScore = 0;
    //     Action best = Action::Wait();
    // 
    //     for (auto p : make_IteratorWithIndex(ALL(laneSPieces))) {
    //         const auto& piece = p.second;
    //         auto i = p.first;
    //         if (piece.restRequiredHeatTurnCount() != 1) continue;
    //         int score = (piece.score());
    //         if (bestScore >= score) continue;
    //         for (Vector2i vec : WipeScan(oven.height() + 1 - piece.height(), oven.width() + 1 - piece.width(), mem::ScanMode)) {
    //             if (oven.isAbleToPut(piece.to_piece(), vec)) {
    //                 bestScore = score;
    //                 best = Action::Put(CandidateLaneType_Small, i, vec);
    //                 break;
    //             }
    //         }
    //     }
    //     if (bestScore > 0) return best;
    // }



    if (mem::mySmallCommandQueue.empty()) {

        auto placements = algo::solvePlacementPiece2(oven.width(), oven.height(), bakingPieces, laneSPieces, 1000, timeLeft);

        // algo::replacePiece(oven.ovenWidth(), oven.ovenHeight(), bakingPieces, laneSPieces, placements);

        sort(ALL(placements), [&](const algo::History::value_type& p1, const algo::History::value_type& p2) {
            int x1 = solveLanePieces(p1.first).restRequiredHeatTurnCount()-1,
                x2 = solveLanePieces(p2.first).restRequiredHeatTurnCount()-1;
            return x1/3 == x2/3 ? x1 > x2 : x1 < x2;
        });
        // placements.sort([&laneSPieces](const algo::History::value_type& p1, const algo::History::value_type& p2) {
        //     int x1 = laneSPieces[p1.first].requiredHeatTurnCount() - 1,
        //         x2 = laneSPieces[p2.first].requiredHeatTurnCount() - 1;
        //     return x1 / 3 == x2 / 3 ? x1 > x2 : x1 < x2;
        // });
        mem::mySmallCommandQueue = move(placements);
    }

    iterate(it, mem::mySmallCommandQueue.begin(), mem::mySmallCommandQueue.end()) {
        auto p = *it;
        auto& piece = solveLanePieces(p.first);
        if (oven.isAbleToPut(piece.to_piece(), p.second)) {
            mem::mySmallCommandQueue.erase(it);

            auto target = mem::piecesManager.id2raw(piece.id());
            return onDecideAction(Action::Put(switchLaneType[target.first], target.second, p.second));
        }
    }

    return onDecideAction(Action::Wait());
}

}
// EOF