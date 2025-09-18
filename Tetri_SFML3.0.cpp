#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <ctime>
#include <array>
#include <random>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;
using namespace sf;

static constexpr int COL = 10;  //列数10
static constexpr int ROW = 20;  //行数20
static constexpr int CELL_SIZE = 40;//单元格大小40
static constexpr int BORDER = 5;    //边框大小5

//单元格
class Cell {
public:
    int x, y; 
};

//方块形状
const int SHAPES[7][4] = {
    {1,5,9,13}, //I
    {5,6,9,10}, //O
    {4,5,9,10}, //Z
    {6,7,9,10}, //S
    {1,5,9,10}, //L
    {2,6,9,10}, //J
    {5,8,9,10}  //T
};

//方块颜色，0表示背景色
const Color COLORS[8] = {
    Color{66,66,66},
    Color{0,240,240}, 
    Color{240,240,0}, 
    Color{160,0,240},
    Color{0,240,0}, 
    Color{240,0,0}, 
    Color{0,0,240}, 
    Color{240,160,0}
};

//方块类
class Block
{
public:
    int id, x, y;
    array<Cell, 4> blocks;

    //默认构造
    Block() : id(0), x(COL / 2 - 2), y(0), blocks{} {};

    //生成方块
    void getBlock(int id)
    {
        this->id = id + 1;  //id从1开始
        for (int i = 0; i < 4; i++)
        {
            int x = SHAPES[id][i];
            blocks[i] = Cell{x % 4, x / 4};
        }
    }

    //旋转方块(顺时针旋转90度)
    void rotate()
    {
        for (auto &c : blocks)
        {
            //坐标(x, y) -> (y, 3-x)
            c = [&c]() {return Cell{c.y, 3 - c.x};}();
        }
    }

};

//7-Bag 随机生成器
struct Bag7 {
    vector<int> bag;    // 当前包
    mt19937 rng{ random_device{}() }; // 随机引擎

    // 返回下一个方块 ID
    int next() {
        if (bag.empty()) {
            // 填充 0~6 并随机打乱
            bag = {0,1,2,3,4,5,6};
            shuffle(bag.begin(), bag.end(), rng);
        }
        int sid = bag.back(); 
        bag.pop_back();
        return sid;
    }
};

//游戏主控类
class Game
{
public:
    Block curr, next;    //当前方块和下一个方块
    array<array<int, COL>, ROW> grid;  //游戏网格
    Bag7 bag;   //随机生成
    int score, lines, level;    //分数、消除行数、难度等级
    bool gameover;  //游戏是否结束
    bool pause;     //游戏是否暂停
    Clock fallClock;    //下落计时器
    double fallTime;    //下落间隔时间
    Sound* clearSoundPtr = nullptr;  //指向消除音效
    

public:
    //默认构造
    Game()
    {
        //初始化
        this->init();
        //生成方块
        this->spawn();
    }

    //初始化游戏
    void init()
    {
        //初始化成员属性
        this->grid = {};
        this->score = 0;
        this->lines = 0;
        this->level = 1;
        //初始化游戏状态
        this->gameover = false;
        this->pause = false;
        this->fallTime = 0.8;
        //初始化方块
        this->next.getBlock(this->bag.next());
    }

    //重置游戏
    void reset()
    {
        //初始化
        this->init();
        //重置时间
        this->fallClock.restart();
        //生成方块
        this->spawn();
    }

    //生成方块
    void spawn()
    {
        this->curr = this->next;
        this->next.getBlock(this->bag.next());
        curr.y = -2;    //顶部生成
        this->fallClock.restart();
        
        // 添加游戏结束检查
        if (isCollide(curr)) {
            gameover = true;
        }
    }

    //碰撞检测
    bool isCollide(const Block &b) const
    {
        for (auto &i: b.blocks)
        {
            int gx = i.x + b.x;
            int gy = i.y + b.y;
            //检测越界
            if (gx < 0 || gx >= COL || gy >= ROW) return true;
            //检测方块
            if (this->grid[gy][gx] && gy >= 0) return true;
        }
        return false;
    }
    
    //检测到碰撞后的流程
    //固定方块->消除完整行->更新分数->生成新方块

    //固定方块
    void lockBlock(const Block &b)
    {
        for (auto &i: b.blocks)
        {
            int gx = i.x + b.x;
            int gy = i.y + b.y;
            //检测边界
            if (gx >= 0 && gx < COL && gy >= 0 && gy < ROW)
            {
                this->grid[gy][gx] = b.id;
            }
        }
    }

    //消除完整行
    int clearLines() {
        int write = ROW - 1;//指定写入行，从最底端开始
        int cleared = 0;    //记录清除行数用于返回
        for (int r = ROW - 1; r >= 0; r--) 
        {
            //判断当前行是否已满
            bool full = true;
            for (int c = 0; c < COL; c++)
            {
                if (grid[r][c] == 0) 
                { 
                    full = false; 
                    break; 
                }
            }
            //未满
            if (!full) 
            {
                //未满行覆盖写入行，write上移
                if (write != r) grid[write] = grid[r];
                write--;
            }
            //已满记录行数
            else cleared++;
        }
        //清空空行
        for (int r = write; r >= 0; r--) grid[r].fill(0);
        return cleared;
    }

    //固定方块后更新分数并生成新方块(完整流程)
    void updateScore()
    {
        this->lockBlock(curr);
        int cl = clearLines();
        if (cl)
        {
            static const int lineScore[5] = {0,100,300,500,800};
            this->score += lineScore[cl] * level;
            this->lines += cl;
            //每消除10行，难度等级加1，最高10级
            this->level = min(1 + this->lines / 10, 10);
            this->fallTime = 0.8 - (level - 1) * 0.05;
            //播放消除音效
            if (clearSoundPtr)
            {
                clearSoundPtr->stop();   // 停止当前播放（可避免叠加）
                clearSoundPtr->play();   // 播放
            }
        }
        this->spawn();
    }
    
    //更新位置(自动下落)
    void updatePosition(double dt)
    {
        if (pause || gameover) return;
        //时间达到预设的时间则下落
        if (fallClock.getElapsedTime().asSeconds() >= fallTime) {
            fallClock.restart();
            curr.y++;
            // 检查是否发生碰撞
            if (isCollide(curr)) {
                // 撤销移动
                curr.y--;
                this->updateScore();
            }
        }
    }

    //移动方块
    void move(int dx)
    {
        curr.x += dx;
        if (isCollide(curr)) curr.x -= dx;
    }

    //软降落
    void softDrop()
    {
        curr.y++;
        if (isCollide(curr)){
            curr.y--;
            this->updateScore();
        }
    }

    //硬降落
    void hardDrop()
    {
        while (!isCollide(curr)) curr.y++;
        curr.y--;
        this->updateScore();
    }

    //旋转方块
    void rotate()
    {
        Block tmp = curr;
        tmp.rotate();
        if (!isCollide(tmp))
        {
            curr = tmp;
            return;
        }
        //墙蹬：方块旋转后，如果发生碰撞，则尝试墙蹬
        const int kicks[4] = {1, -1, 2, -2};
        for (auto k : kicks)
        {
            tmp = curr;
            tmp.rotate();
            tmp.x += k;
            if (!isCollide(tmp))
            {
                curr = tmp;
                return;
            }
        }
    }
    
};

int main()
{
    const int WIDTH = (COL + 8)*CELL_SIZE + BORDER*2;//窗口宽度
    const int HEIGHT = ROW * CELL_SIZE + BORDER*2; //窗口高度

    //创建窗口
    RenderWindow window(VideoMode(Vector2u(WIDTH, HEIGHT)), "俄罗斯方块");
    
    //设置帧率上限
    window.setFramerateLimit(60);

    //加载音频
    Music bgm;  //背景音乐
    Music gameoverMusic;    //游戏结束音乐
    SoundBuffer clearBuffer;//消除音效缓存
    Sound clearSound(clearBuffer);   //消除音效

    //加载背景音乐
    if (!bgm.openFromFile("res/bgm.mp3")) {
        cerr << "无法加载背景音乐！" << endl;
    } else {
        bgm.setLooping(true);
        bgm.setVolume(40.f);
        bgm.play();
    }

    //加载游戏结束音乐
    if (!gameoverMusic.openFromFile("res/gameover.mp3")) {
        cerr << "无法加载游戏结束音乐！" << endl;
    } else {
        gameoverMusic.setLooping(false);
        gameoverMusic.setVolume(60.f);
    }

    //加载消除音效
    if (!clearBuffer.loadFromFile("res/clear.mp3")) {
        cerr << "无法加载消除音效！" << endl;
    } else {
        clearSound.setBuffer(clearBuffer);
        clearSound.setVolume(80.f);
    }

    Game game;  //创建游戏对象
    game.clearSoundPtr = &clearSound;  //传入音效指针
    static bool gomPlaying = false;    //游戏结束音乐是否播放
    
    //游戏主循环
    while (window.isOpen())
    {
        //---------- 处理事件 ----------
        while (const optional e = window.pollEvent())
        {
            //窗口关闭事件
            if (e->is<Event::Closed>())
                window.close();
            
            //游戏暂停事件
            if (game.pause && bgm.getStatus() == SoundSource::Status::Playing)
                bgm.pause();
            else if (!game.pause && !game.gameover 
                && bgm.getStatus() == SoundSource::Status::Paused)
                bgm.play();
                
            //键盘事件
            if (e->is<Event::KeyPressed>())
            {
                const auto& key = e->getIf<Event::KeyPressed>();
                if (!key) continue;

                //控制事件
                switch (key->code)
                {
                    case Keyboard::Key::Escape: // Esc退出
                        window.close();
                        break;
                    case Keyboard::Key::P:  // P暂停
                        game.pause = !game.pause;
                        break;
                    case Keyboard::Key::R:  // R重新开始
                        game.reset();
                        bgm.play();  //重新播放背景音乐
                        gameoverMusic.stop();
                        break;
                    default:
                        break;
                }
                if (game.pause || game.gameover) continue;

                //操作事件
                switch(key->code)
                {
                    case Keyboard::Key::Up:
                    case Keyboard::Key::W:
                        game.rotate();
                        break;
                    case Keyboard::Key::Down:
                    case Keyboard::Key::S:
                        game.softDrop();
                        break;
                    case Keyboard::Key::Space:
                        game.hardDrop();
                        break;
                    case Keyboard::Key::Left:
                    case Keyboard::Key::A:
                        game.move(-1);
                        break;
                    case Keyboard::Key::Right:
                    case Keyboard::Key::D:
                        game.move(1);
                        break;
                    default:
                        break;
                }
            }
        }

        //---------- 更新游戏逻辑 ----------
        //每次处理操作事件时都会调用碰撞检测
        //每次控制方块降落时都会调用固定方块和更新分数的函数
        //更新分数会调用消除完整行的函数
        //因此这里仅需调用自动下落函数即可
        //更新位置(自动下落)
        game.updatePosition(game.fallClock.getElapsedTime().asSeconds());

        //游戏结束音乐逻辑（只播放一次）
        if (game.gameover && gomPlaying == false
            && gameoverMusic.getStatus() == SoundSource::Status::Stopped) {
            if (bgm.getStatus() == SoundSource::Status::Playing)
                bgm.stop();
            gameoverMusic.play();
            gomPlaying = true;
        }
        
        //---------- 绘制游戏画面 ----------
        //绘制背景
        window.clear(Color{20,20,30});

        //绘制游戏井
        RectangleShape well;
        well.setPosition(Vector2f(float(BORDER),float(BORDER)));  //左上角位置
        well.setSize(Vector2f(float(COL*CELL_SIZE), float(ROW*CELL_SIZE)));//井宽高
        well.setFillColor(Color{8,8,12});  //井颜色
        well.setOutlineThickness(float(BORDER));   //边框宽度
        well.setOutlineColor(Color{60,60,80}); //边框颜色
        window.draw(well);

        //创建单元格模版
        auto makeCell = []()
        {
            RectangleShape r(Vector2f(CELL_SIZE-1.0, CELL_SIZE-1.0));
            r.setOutlineThickness(1.0);
            r.setOutlineColor(Color{15,15,15});
            return r;
        };
        //创建单元格
        auto cell = makeCell();
        //绘制单元格(网格)
        for (int r = 0; r < ROW; r++) {
            for (int c = 0; c < COL; c++) {
                cell.setPosition(Vector2f(float(BORDER + c*CELL_SIZE), float(BORDER + r*CELL_SIZE)));
                int id = game.grid[r][c];
                if (id >= 0 && id < int(size(COLORS))) cell.setFillColor(COLORS[id]);
                window.draw(cell);
            }
        }

        //绘制当前方块
        for (auto &b : game.curr.blocks)
        {
            int gx = game.curr.x + b.x, gy = game.curr.y + b.y;
            // 修复方块位置计算，确保只绘制在游戏区域内的部分
            if (gy >= 0) {
                cell.setPosition(Vector2f(float(BORDER + gx*CELL_SIZE), float(BORDER + gy*CELL_SIZE)));
                cell.setFillColor(COLORS[game.curr.id]);
                window.draw(cell);
            }
        }

        //---------- 绘制右侧UI ----------
        //加载支持中文的字体
        Font font;
        vector<string> fontPaths =
        {
            //Windows常见字体
            "C:/Windows/Fonts/msyh.ttc",   //微软雅黑
            "C:/Windows/Fonts/simhei.ttf", //黑体
            "C:/Windows/Fonts/simsun.ttc", //宋体

            //Linux常见字体
            "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc", //文泉驿正黑
            "/usr/share/fonts/truetype/arphic/ukai.ttc",   //文鼎PL UKai

            //macOS常见字体
            "/System/Library/Fonts/PingFang.ttc" //苹方
        };
        bool fontLoaded = false;
        for (auto &p : fontPaths)
        {
            if (font.openFromFile(p))
            { 
                fontLoaded = true;
                // cout << "Loaded font: " << p << endl;
                break;
            }
        }
        if (!fontLoaded)
        {
            cerr << "无法加载中文字体，请检查系统是否有合适的字体文件。" << endl;
        }

        //UI横坐标
        double px = double((COL + 2) * CELL_SIZE + BORDER * 2);

        //创建文字函数(支持中文)
        auto drawText = [&](const wstring &text, double x, double y,
            unsigned fontSize = 20, Color color = Color::White)
        {
            //先把 wstring 转成 UTF-32（sf::String 内部用的就是 UTF-32）
            String utf32 = String::fromUtf32(text.begin(), text.end());
            Text t(font, utf32, fontSize);
            t.setPosition(Vector2f(x, y));
            t.setFillColor(color);
            window.draw(t);
        };

        //绘制分数、消除行数、难度等级、下一个
        drawText(L"玩家得分：" + std::to_wstring(game.score), px, 50);
        drawText(L"消除行数：" + std::to_wstring(game.lines), px, 100);
        drawText(L"难度等级：" + std::to_wstring(game.level), px, 150);
        drawText(L"下一个：", px, 200);

        //绘制下一个方块
        //创建方框，用于绘制方块
        RectangleShape box(Vector2f(float(4*CELL_SIZE), float(4*CELL_SIZE)));
        box.setPosition(Vector2f(px, 250)); //设置方框位置
        box.setFillColor(Color{12,12,18});  //设置方框颜色
        box.setOutlineThickness(float(BORDER));//设置方框线条宽度
        box.setOutlineColor(Color{60,60,80});//设置方框线条颜色
        window.draw(box);
        //绘制方块
        for (auto &c : game.next.blocks)
        {
            cell.setPosition(Vector2f(px + c.x*CELL_SIZE, float(250 + c.y*CELL_SIZE)));
            cell.setFillColor(COLORS[game.next.id]);
            window.draw(cell);
        }

        //绘制操作说明
        double ty = 300 + CELL_SIZE*4;  //起始纵坐标
        drawText(L"操作说明：", px, ty);
        drawText(L"按A、D或←、→键移动", px, ty + 30);
        drawText(L"按W或↑键旋转", px, ty + 60);
        drawText(L"按S或↓键加速下落", px, ty + 90);
        drawText(L"按空格键落下", px, ty + 120);
        drawText(L"按P键暂停", px, ty + 150);
        drawText(L"按R键重启", px, ty + 180);
        drawText(L"按ESC键退出", px, ty + 210);

        //绘制游戏状态
        if (game.pause && !game.gameover)
        {
            drawText(L"游戏暂停", px, HEIGHT - 80, 30, Color::Red);
        }
        if (game.gameover)
        {
            drawText(L"游戏结束", px, HEIGHT - 80, 30, Color::Red);
        }
        
        window.display();
    }
    
    return 0;
}