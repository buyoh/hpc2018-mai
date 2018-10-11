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


/*


TODO:

- 90以上のピースを無視する




*/

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
        constexpr DreamcastScan(int _h, int _w) :Height(_h), Width(_w) {}
        struct Iterator {
            int h, w; // width
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
        constexpr inline Iterator begin() const { return Iterator(Height, Width, 0, 0); }
        constexpr inline Iterator end() const { return Iterator(0, 0, 0, 0); }
    };


    class WipeScan {
        const int Height, Width;
        const bool Tr;
    public:
        struct Iterator {
            const int h, w; // width
            int i, j; // index
            const bool tr;
            constexpr Iterator(int _h, int _w, int _i, int _j, bool _tr) :h(_h), w(_w), i(_i), j(_j), tr(_tr) {}

            inline Vector2i operator*() const {
                return Vector2i(j & 1 ? w - 1 - (j >> 1) : (j >> 1), i & 1 ? h - 1 - (i >> 1) : (i >> 1));
            }
            inline Iterator& operator++() noexcept {
                if (!tr && ++j >= w) j = 0, ++i;
                else if (tr && ++i >= h) i = 0, ++j;
                return *this;
            }
            inline bool operator!=(const Iterator& another) const noexcept {
                return w != another.w || i != another.i || j != another.j;

            }
        };
        constexpr WipeScan(int _h, int _w, bool _tr = false) :Height(_h), Width(_w), Tr(_tr) {}
        constexpr inline Iterator begin() const { return Iterator(Height, Width, 0, 0, Tr); }
        constexpr inline Iterator end() const { return Iterator(Height, Width, Tr ? 0 : Height, Tr ? Width : 0, Tr); }
    };



    class LScan {
        const int Height, Width;
    public:
        struct Iterator {
            int mini, maxi, sw; // width
            int i, o = 0; // index
            constexpr Iterator(int _mini, int _maxi, int _sw, int _i, int _o) :mini(_mini), maxi(_maxi), sw(_sw), i(_i), o(_o) {}

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
        constexpr LScan(int _h, int _w) :Height(_h), Width(_w) {}
        constexpr inline Iterator begin() const { return Iterator(min(Height, Width), max(Height, Width), Height > Width, 1, 0); }
        constexpr inline Iterator end() const { return Iterator(min(Height, Width), max(Height, Width), Height > Width, 1, min(Height, Width)); }
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
        // static mt19937_64 randdev(8901016);
        static XorShift randdev;

        // 
        template<typename T> inline T rand(T l, T h) { return uniform_int_distribution<T>(l, h)(randdev); }
        template<> inline double rand<double>(double l, double h) { return uniform_real_distribution<double>(l, h)(util::randdev); }
        template<> inline float rand<float>(float l, float h) { return uniform_real_distribution<float>(l, h)(util::randdev); }

        class MyPiece {
            int x_, y_, w_, h_, remTurn_, s_, id_;
        public:
            MyPiece(const Piece& p, int id = -1) :
                x_(p.pos().x), y_(p.pos().y),
                w_(p.width()), h_(p.height()),
                remTurn_(p.restRequiredHeatTurnCount()), s_(p.score()), id_(id) {}
            MyPiece(int _x, int _y, int _w, int _h) :
                x_(_x), y_(_y),
                w_(_w), h_(_h),
                remTurn_(0), s_(0) {}
            inline int x() const noexcept { return x_; }
            inline int y() const noexcept { return y_; }
            inline int width() const noexcept { return w_; }
            inline int height() const noexcept { return h_; }
            inline int restRequiredHeatTurnCount() const noexcept { return remTurn_; }
            inline int score() const noexcept { return s_; }
            inline int id() const noexcept { return id_; }

            inline MyPiece& moveTo(int _x, int _y) noexcept { x_ = _x; y_ = _y; return *this; }

            // inline Vector2i pos() const { return Vector2i(x_, y_); }
        };

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
                    begin->pos().x, begin->pos().y, begin->width(), begin->height()
                )) {
                    return move(begin);
                }
            }
            return move(begin);
        }

        /// @return end iterator -> NO / valid iterator -> YES
        template<typename ITER>
        //using ITER = vector<Piece>::const_iterator;
        inline ITER isIntersectPieces(ITER begin, ITER end, const Vector2i& pos, const Piece& piece) {
            return isIntersectPieces(begin, end, pos.x, pos.y, piece.width(), piece.height());
        }

        inline bool isIntersectPieces(const Piece& p1, const Piece& p2) {
            return util::isIntersect(
                p1.pos(), p1.width(), p1.height(),
                p2.pos(), p2.width(), p2.height()
            );
        }
        inline bool isIntersectPieces(const Vector2i& v1, const Piece& p1, const Vector2i& v2, const Piece& p2) {
            return util::isIntersect(
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

        //template<typename ITER>
        using ITER = vector<Piece>::const_iterator;
        inline int distancePieces(ITER begin, ITER end, int ovenWidth, int ovenHeight, int px, int py) {
            int dx = max(ovenWidth - 1 - px, px),
                dy = max(ovenHeight - 1 - py, py);
            for (; begin != end; ++begin) {
                dx = min<int>(dx, max<int>(begin->pos().x - px, px - (begin->pos().x + begin->width())));
                dy = min<int>(dy, max<int>(begin->pos().y - py, py - (begin->pos().y + begin->height())));
                if (dx < 0 || dy < 0) return -1;
            }
            return max(dx, dy);
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



        History solvePlacementPiece2(const int width, const int height, const vector<Piece>& placedPieces, const vector<Piece>& pieces, int maxLoopcount) {
            
            vector<Tag<int, const Piece*>> shuffler;
            shuffler.reserve(pieces.size());
            for (auto p : make_IteratorWithIndex(ALL(pieces)))
                shuffler.emplace_back(p.first, &p.second);

            int bestScore = 0;
            History best, currResult;
            int bestFixTime = 0;

            sort(ALL(shuffler));

            vector<Piece> placed;
            repeat(loopcnt, maxLoopcount) {
                shuffle(ALL(shuffler), util::randdev);
                // list<Piece> placed(ALL(placedPieces));
                placed = placedPieces;
                //placed.reserve(placedPieces.size() + pieces.size());2458764

                int currScore = 0;
                currResult.clear();

                repeat(extraChance, 4) {
                    for (auto picked : shuffler) {

                        // 今置こうとしている
                        auto& piece = *picked.second;
                        // 直前の検証で重なった
                        auto currIsec = placed.end();


                        auto test = [&](Vector2i vec) {

                            if (currIsec != placed.end()) {
                                if (util::isIntersectPieces(currIsec->pos(), *currIsec, vec, piece))
                                    return false;
                            }

                            auto isec = util::isIntersectPieces(ALL(placed), vec, piece);

                            if (isec == placed.end()) {
                                // 置く．
                                placed.emplace_back(vec, piece.width(), piece.height(), 114, 514);
                                currScore += piece.score();
                                if (extraChance == 0)
                                    currResult.emplace_back(picked.first, vec);
                                return true;
                            }
                            else {
                                currIsec = move(isec);
                            }
                            return false;
                        };
                        
                        // if (mem::ScanMode == 2) {
                        //     for (auto vec :
                        //         LScan(height + 1 - piece.height(), width + 1 - piece.width())) {
                        //         vec.x = width - piece.width() - vec.x;
                        //         vec.y = height - piece.height() - vec.y;
                        //         if (test(vec)) break;
                        //     }
                        // }
                        // else
                        {
                            for (auto vec :
                                WipeScan(height + 1 - piece.height(), width + 1 - piece.width(), mem::ScanMode ^ !(randdev() & 31))) {
                                if (test(vec)) break;
                            }
                        }
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

            return best;
        }


        History solvePlacementPiece(const int width, const int height, const vector<Piece>& placedPieces, const vector<Piece>& pieces) {

            vector<Tag<int, const Piece*>> shuffler;
            shuffler.reserve(pieces.size());
            for (auto p : make_IteratorWithIndex(ALL(pieces)))
                shuffler.emplace_back(p.first, &p.second);

            int bestScore = 0;
            History best;

            sort(ALL(shuffler), [](const decltype(shuffler)::value_type& l, const decltype(shuffler)::value_type& r) {
                return l.second->score() > r.second->score();
            });

            function<void(vector<int>& indices, vector<Piece>& placed, History& currResult, int currScore)> dfs =
                [&](vector<int>& indices, vector<Piece>& placed, History& currResult, int currScore) -> void {

                if (bestScore < currScore) {
                    bestScore = currScore;
                    best = currResult;
                }
                bool empty = true;
                repeat(i, indices.size()) {
                    int idx = indices[i];
                    if (idx < 0) continue;
                    empty = false;

                    auto& picked = shuffler[idx];

                    // 今置こうとしている
                    auto& piece = *picked.second;
                    // 直前の検証で重なった
                    auto currIsec = placed.end();


                    auto test = [&](Vector2i vec) {

                        if (currIsec != placed.end()) {
                            if (util::isIntersectPieces(currIsec->pos(), *currIsec, vec, piece))
                                return false;
                        }

                        auto isec = util::isIntersectPieces(ALL(placed), vec, piece);

                        if (isec == placed.end()) {
                            // 置く．
                            placed.emplace_back(vec, piece.width(), piece.height(), 114, 514);
                            currScore += piece.score();
                            currResult.emplace_back(picked.first, vec);
                            indices[i] = -1;

                            dfs(indices, placed, currResult, currScore);

                            indices[i] = idx;
                            currResult.pop_back();
                            currScore -= piece.score();
                            placed.pop_back();
                            return true;
                        }
                        else {
                            currIsec = move(isec);
                        }

                        return false;
                    };

                    // if (mem::ScanMode == 2) {
                    //     for (auto vec :
                    //         LScan(height + 1 - piece.height(), width + 1 - piece.width())) {
                    //         if (test(vec)) break;
                    //     }
                    // }
                    // else
                    {
                        for (auto vec :
                            WipeScan(height + 1 - piece.height(), width + 1 - piece.width(), mem::ScanMode ^ !(randdev() & 31))) {
                            if (test(vec)) break;
                        }
                    }
                }

                if (empty) {
                    // extra chance !!

                    auto ex_placed = placed;
                    int ex_currScore = currScore;

                    repeat(lop, 3) {
                        for (auto picked : shuffler) {

                            // 今置こうとしている
                            auto& piece = *picked.second;
                            // 直前の検証で重なった
                            auto currIsec = ex_placed.end();

                            //Vector2i lastPos(-1, -1);
                            for (auto vec : WipeScan(height + 1 - piece.height(), width + 1 - piece.width(), true)) {
                                //int x = yx.first, y = yx.second; // swap

                                if (currIsec != ex_placed.end()) {
                                    if (util::isIntersectPieces(currIsec->pos(), *currIsec, vec, piece))
                                        continue;
                                }

                                auto isec = util::isIntersectPieces(ALL(placed), vec, piece);

                                if (isec == ex_placed.end()) {
                                    // 置く．
                                    //lastPos = vec;
                                    ex_placed.emplace_back(vec, piece.width(), piece.height(), 114, 514);
                                    ex_currScore += piece.score();

                                    break;
                                }
                                else {
                                    currIsec = move(isec);
                                }
                            }
                        }
                    }
                    if (bestScore < ex_currScore) {
                        bestScore = ex_currScore;
                        best = currResult;
                    }
                }
            };

            vector<Piece> placed = placedPieces;
            vector<int> indices(shuffler.size()); iota(ALL(indices), 0);
            History currResult;
            dfs(indices, placed, currResult, 0);


            analysis::counter[vector<int>{1, (int)best.size()}]++;
            return best;
        }


        void generateListOfEmptyCell() {

        }


        // void replacePiece(const int width, const int height, const vector<Piece>& placedPieces, const vector<Piece>& pieces, History& history) {
        //     
        //     int bestscore = 0;
        //     {
        //         vector<Piece> placed;
        //         placed = placedPieces;
        //         for (auto& p : history)
        //             placed.emplace_back(p.second, pieces[p.first].width(), pieces[p.first].height(), 114, 514);
        // 
        //         repeat(x, width) {
        //             repeat(y, height) {
        //                 bestscore += max(0, util::distancePieces(ALL(placed), width, height, x, y));
        //             }
        //         }
        //     }
        // 
        //     int ana_prevScore = bestscore;
        // 
        //     repeat(tryloopcnt, 300) {
        // 
        //         History currHistory; currHistory.reserve(history.size());
        // 
        //         vector<Piece> placed;
        //         placed.reserve(placedPieces.size() + history.size());
        //         placed = placedPieces;
        // 
        //         bool fail = true;
        //         for (auto& p : history) {
        //             auto& piece = pieces[p.first];
        // 
        //             int x, y;
        //             fail = true;
        //             repeat(lop2, 100) {
        //                 x = util::rand<int>(0, width - 1);
        //                 y = util::rand<int>(0, height - 1);
        //                 auto isec = util::isIntersectPieces(ALL(placed), x, y, piece.width(), piece.height());
        //                 if (isec == placed.end()) {
        //                     fail = false;
        //                     break;
        //                 }
        //             }
        //             if (fail) break;
        //             placed.emplace_back(Vector2i(x, y), piece.width(), piece.height(), 114, 514);
        //         }
        //         
        //         if (fail) continue;
        // 
        //         int score = 0;
        //         repeat(x, width) {
        //             repeat(y, height) {
        //                 score += max(0, util::distancePieces(ALL(placed), width, height, x, y));
        //             }
        //         }
        //         if (bestscore < score) {
        //             bestscore = score;
        //             history = move(currHistory);
        //         }
        //     }
        // 
        //     analysis::counter[vector<int>{bestscore - ana_prevScore}]++;
        // }


        // void expandPiecesRandom(vector<Piece>& placedPieces) {
        // }
    }

//------------------------------------------------------------------------------

    namespace mem {
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

    laneS.recipe().maxSampleEdgeLength();

    if (aStage.turn() == 0) {
        mem::ScanMode = 0; // TODO: 上手いこと実装
        int hLong = 0, vLong = 0;
        for (auto& p : laneS.pieces()) hLong += p.width(), vLong += p.height();
        for (auto& p : laneL.pieces()) hLong += p.width(), vLong += p.height();
        if (hLong * 3 < vLong * 2) mem::ScanMode = 1;
        if (vLong * 3 < hLong * 2) mem::ScanMode = 0;
    }

    vector<Piece> bakingPieces(ALL(oven.bakingPieces()));
    vector<Piece> bakingUnignorablePieces;
    for (auto& p : bakingPieces) {
        if (p.restRequiredHeatTurnCount()+(aStage.turn()%4) >= 8) bakingUnignorablePieces.push_back(p);
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
        // TODO
        // vector<Piece> usePieces = laneLPieces;
        // vector<int> usePiecesIndexS;
        // for (auto p : make_IteratorWithIndex(ALL(laneSPieces))) {
        //     if (p.second.restRequiredHeatTurnCount() > 10)
        //         usePiecesIndexS.push_back(p.first),
        //         usePieces.push_back(p.second);
        // }
        auto placements = algo::solvePlacementPiece(oven.width(), oven.height(), bakingUnignorablePieces, laneLPieces);

        if (!placements.empty()) {

            sort(ALL(placements), [&laneLPieces](const algo::History::value_type& p1, const algo::History::value_type& p2) {
                return laneLPieces[p1.first].requiredHeatTurnCount() > laneLPieces[p2.first].requiredHeatTurnCount();
            });
            // placements.sort([&laneLPieces](const algo::History::value_type& p1, const algo::History::value_type& p2) {
            //     return laneLPieces[p1.first].requiredHeatTurnCount() > laneLPieces[p2.first].requiredHeatTurnCount();
            // });
            mem::mySmallCommandQueue.clear(),
            mem::myLargeCommandQueue = move(placements);
        }
    }
    else{
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

    {
        int bestScore = 0;
        Action best = Action::Wait();

        for (auto p : make_IteratorWithIndex(ALL(laneSPieces))) {
            const auto& piece = p.second;
            auto i = p.first;
            if (piece.restRequiredHeatTurnCount() != 1) continue;
            int score = (piece.score());
            if (bestScore >= score) continue;
            for (Vector2i vec : WipeScan(oven.height() + 1 - piece.height(), oven.width() + 1 - piece.width(), mem::ScanMode)) {
                if (oven.isAbleToPut(piece, vec)) {
                    bestScore = score;
                    best = Action::Put(CandidateLaneType_Small, i, vec);
                    break;
                }
            }
        }
        if (bestScore > 0) return best;
    }



    if (mem::mySmallCommandQueue.empty()) {

        auto placements = algo::solvePlacementPiece2(oven.width(), oven.height(), bakingPieces, laneSPieces, 700);

        // algo::replacePiece(oven.width(), oven.height(), bakingPieces, laneSPieces, placements);

        sort(ALL(placements), [&laneSPieces](const algo::History::value_type& p1, const algo::History::value_type& p2) {
            int x1 = laneSPieces[p1.first].requiredHeatTurnCount()-1,
                x2 = laneSPieces[p2.first].requiredHeatTurnCount()-1;
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
        auto& piece = laneSPieces[p.first];
        if (oven.isAbleToPut(piece, p.second)) {
            for (auto& q : mem::mySmallCommandQueue)
                if (q.first > p.first) --q.first;
            mem::mySmallCommandQueue.erase(it);
            return Action::Put(CandidateLaneType_Small, p.first, p.second);
        }
    }

    return Action::Wait();
}

}
// EOF