#include<iostream>
#include<algorithm>
#include<vector>
#include<stdlib.h>
#include<time.h>
#include<GLFW/glfw3.h>
using namespace std;
//settings
int MUTATION_CHANCE = 33;
double MAX_MUTATION_STRENGTH = 0.05;
int MAX_XY = 255;
int MAX_RADIUS = MAX_XY / 4;
const int MAX_COLOUR = 255;
const int MAX_ALPHA = 100;
int MAX_FIGURES = 200;
int MAX_DEPTH = MAX_RADIUS*100;
int CROSS_CHANCE = 75;
int KEEP_SORTED = 2; //0-2
int CONST_ALPHA = 100;
int KEEP_ALPHA = 0; //0-3
int DRAW_BACKGROUND = 1; //0-1
int KEEP_DEPTH = 2; //0-2
int FIGURE_TYPE = 0; //0-2
int MAX_IMAGES = 10;
int CHOOSE3 = 0; //0-1
int WINDOW_SIZE = 400;

int mutation(int a, int m)
{
    int mut_str = 0, mut;
    mut = rand() % 100;
    if (mut<MUTATION_CHANCE)
    {
        mut_str = rand() % (2 * int(MAX_MUTATION_STRENGTH*m) + 1) - int(MAX_MUTATION_STRENGTH*m);
    }
    a += mut_str;
    if (a<0) a = 0;
    if (a>m) a = m;
    return a;
}
struct figure
{
    int x, y, r1, r2, r, g, b, a, d;
    figure duplicate()
    {
        figure newf;
        newf.x = mutation(x, MAX_XY);
        newf.y = mutation(y, MAX_XY);
        newf.r1 = mutation(r1, MAX_RADIUS);
        newf.r2 = mutation(r2, MAX_RADIUS);
        newf.r = mutation(r, MAX_COLOUR);
        newf.g = mutation(g, MAX_COLOUR);
        newf.b = mutation(b, MAX_COLOUR);
        newf.a = mutation(a, MAX_ALPHA);
        newf.d = mutation(d, MAX_DEPTH);
        if (KEEP_ALPHA >= 2) newf.a = CONST_ALPHA;
        if (KEEP_DEPTH >= 2) newf.d = newf.r1*newf.r2*MAX_DEPTH / MAX_RADIUS / MAX_RADIUS;
        return newf;
    }
    void randomise()
    {
        x = rand() % (MAX_XY + 1);
        y = rand() % (MAX_XY + 1);
        r1 = rand() % (MAX_RADIUS + 1);
        r2 = rand() % (MAX_RADIUS + 1);
        r = rand() % (MAX_COLOUR + 1);
        g = rand() % (MAX_COLOUR + 1);
        b = rand() % (MAX_COLOUR + 1);
        a = rand() % (MAX_ALPHA + 1);
        d = rand() % (MAX_DEPTH + 1);
        if (KEEP_ALPHA >= 1) a = CONST_ALPHA;
        if (KEEP_DEPTH >= 1) d = r1*r2*MAX_DEPTH / MAX_RADIUS / MAX_RADIUS;
    }
    void zero()
    {
        x = 0;
        y = 0;
        r1 = 0;
        r2 = 0;
        r = 0;
        g = 0;
        b = 0;
        a = 0;
        d = 0;
        if (KEEP_ALPHA >= 1) a = CONST_ALPHA;
        if (KEEP_DEPTH >= 1) d = r1*r2*MAX_DEPTH / MAX_RADIUS / MAX_RADIUS;
    }
    void color(int pr, int pg, int pb, int pa)
    {
        r=pr;
        g=pg;
        b=pb;
        a=pa;
    }
    void setv(int px, int py, int pr1, int pr2, int pr, int pg, int pb, int pa, int pd)
    {
        x = px;
        y = py;
        r1 = pr1;
        r2 = pr2;
        r = pr;
        g = pg;
        b = pb;
        a = pa;
        d = pd;
        if (KEEP_ALPHA >= 2) a = CONST_ALPHA;
        if (KEEP_DEPTH >= 2) d = r1*r2*MAX_DEPTH / MAX_RADIUS / MAX_RADIUS;
    }
    void enter()
    {
        cin >> x >> y >> r1 >> r2 >> r >> g >> b >> a >> d;
        if (KEEP_ALPHA >= 2) a = CONST_ALPHA;
        if (KEEP_DEPTH >= 2) d = r1*r2*MAX_DEPTH / MAX_RADIUS / MAX_RADIUS;
    }
    void print()
    {
        cout<<x<<" "<<y<<" "<<r1<<" "<<r2<<" "<<r<<" "<<g<<" "<<b<<" "<<a<<" "<<d<< "\n";
    }
};
bool operator<(const figure a, const figure b)
{
    if (a.x<b.x) return 1;
    if (a.x==b.x && a.y<b.y) return 1;
    return 0;
}
bool cmpByDepth(const figure a, const figure b)
{
    if (a.d>b.d) return 1;
    return 0;
}
struct image
{
    int r, g, b;
    vector<figure> figures;
    image mutate()
    {
        image newp;
        figure cf;
        int oldsize = figures.size();
        if (oldsize != MAX_FIGURES)
        {
            for (int i = oldsize; i < MAX_FIGURES; i++)
            {
                cf.randomise();
                figures.push_back(cf);
            }
            if (KEEP_SORTED >= 2) sort(figures.begin(), figures.end());
            else if (KEEP_SORTED >= 1) sort(figures.begin()+oldsize, figures.end());
        }
        if (newp.figures.size() != MAX_FIGURES) newp.figures.resize(MAX_FIGURES);
        for (int i = 0; i < MAX_FIGURES; i++)
        {
            newp.figures[i] = figures[i].duplicate();
        }
        newp.r = mutation(r, MAX_COLOUR);
        newp.g = mutation(g, MAX_COLOUR);
        newp.b = mutation(b, MAX_COLOUR);
        if (KEEP_SORTED >= 2) sort(newp.figures.begin(), newp.figures.end());
        return newp;
    }
    image combine(image a)
    {
        image newp;
        figure cf;
        int oldsize = figures.size();
        if (figures.size() != MAX_FIGURES)
        {
            for (int i = figures.size(); i < MAX_FIGURES; i++)
            {
                cf.randomise();
                figures.push_back(cf);
            }
            if (KEEP_SORTED >= 2) sort(figures.begin(), figures.end());
            else if (KEEP_SORTED >= 1) sort(figures.begin() + oldsize, figures.end());
        }
        if (newp.figures.size() != MAX_FIGURES) newp.figures.resize(MAX_FIGURES);
        int cpc = 100;
        vector<int> cross_points;
        while (rand() % 100 < cpc)
        {
            cross_points.push_back(rand() % MAX_FIGURES);
            cpc *= CROSS_CHANCE;
            cpc /= 100;
        }
        sort(cross_points.begin(), cross_points.end());
        int curcp = 0;
        bool side = rand() % 2;
        if (!side)
        {
            newp.r = mutation(r, MAX_COLOUR);
            newp.g = mutation(g, MAX_COLOUR);
            newp.b = mutation(b, MAX_COLOUR);
        }
        else
        {
            newp.r = mutation(a.r, MAX_COLOUR);
            newp.g = mutation(a.g, MAX_COLOUR);
            newp.b = mutation(a.b, MAX_COLOUR);
        }
        for (int i = 0; i < MAX_FIGURES; i++)
        {
            while (curcp < cross_points.size() && i == cross_points[curcp])
            {
                curcp++;
                side = !side;
            }
            if (!side) newp.figures[i] = figures[i].duplicate();
            else newp.figures[i] = a.figures[i].duplicate();
        }
        if (KEEP_SORTED >= 2) sort(newp.figures.begin(), newp.figures.end());
        return newp;
    }
    void zero()
    {
        if (figures.size() != MAX_FIGURES) figures.resize(MAX_FIGURES);
        for (int i = 0; i < MAX_FIGURES; i++)
        {
            figures[i].zero();
        }
        r = 0;
        g = 0;
        b = 0;
        if (KEEP_SORTED >= 1) sort(figures.begin(), figures.end());
    }
    void color(int pr, int pg, int pb, int pa)
    {
        if (figures.size() != MAX_FIGURES) figures.resize(MAX_FIGURES);
        for (int i = 0; i < MAX_FIGURES; i++)
        {
            figures[i].color(pr,pg,pb,pa);
        }
        if (KEEP_SORTED >= 1) sort(figures.begin(), figures.end());
    }
    void randomise()
    {
        if (figures.size() != MAX_FIGURES) figures.resize(MAX_FIGURES);
        for (int i = 0; i < MAX_FIGURES; i++)
        {
            figures[i].randomise();
        }
        r = rand() % (MAX_COLOUR + 1);
        g = rand() % (MAX_COLOUR + 1);
        b = rand() % (MAX_COLOUR + 1);
        if (KEEP_SORTED >= 1) sort(figures.begin(), figures.end());
    }
    void enter()
    {
        if (figures.size() != MAX_FIGURES) figures.resize(MAX_FIGURES);
        cin >> r >> g >> b;
        for (int i = 0; i < MAX_FIGURES; i++)
        {
            figures[i].enter();
        }
        if (KEEP_SORTED >= 2) sort(figures.begin(), figures.end());
    }
    void print()
    {
        figure cf;
        if (figures.size()!=MAX_FIGURES)
        {
            for (int i=figures.size();i<MAX_FIGURES;i++)
            {
                cf.randomise();
                figures.push_back(cf);
            }
        }
        cout<<r<<" "<<g<<" "<<b<<endl;
        for (int i=0;i<MAX_FIGURES;i++)
        {
            figures[i].print();
        }
    }
};

int winner=0;
int stop=0;

//Graphics related functions
void error_callback(int error, const char* description)
{
    cout<<error<<": "<<description<<endl;
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key==GLFW_KEY_1 && action==GLFW_PRESS)
        winner=1;
    if (key==GLFW_KEY_2 && action==GLFW_PRESS)
        winner=2;
    if (CHOOSE3==1 && key==GLFW_KEY_3 && action==GLFW_PRESS)
        winner=3;
    if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
        stop=1;
}
GLFWwindow *window1,*window2,*window3;
string initializeGraphics()
{
    if (!glfwInit())
        return "Unable to initialize library.";
    glfwSetErrorCallback(error_callback);
    return "Initialized successfully.";
}
string createWindows()
{
    window1=glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "Image 1", NULL, NULL);
    if (!window1)
    {
        glfwTerminate();
        return "Unable to open window 1.";
    }
    glfwSetKeyCallback(window1, key_callback);
    window2=glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "Image 2", NULL, NULL);
    if (!window2)
    {
        glfwTerminate();
        return "Unable to open window 2.";
    }
    glfwSetKeyCallback(window2, key_callback);
    if (CHOOSE3==1)
    {
        window3=glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "Image 3", NULL, NULL);
        if (!window3)
        {
            glfwTerminate();
            return "Unable to open window 3.";
        }
        glfwSetKeyCallback(window3, key_callback);
    }
    return "Windows created successfully.";
}
void stopGraphics()
{
    glfwDestroyWindow(window1);
    glfwDestroyWindow(window2);
    glfwTerminate();
}
void drawImage(GLFWwindow* w1, image i1)
{
    float ratio;
    int width, height;
    vector<figure> fs;
    figure curf;

    glfwMakeContextCurrent(w1);
    glfwGetFramebufferSize(w1, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    //background
    glColor3f(double(i1.r)/MAX_COLOUR, double(i1.g)/MAX_COLOUR, double(i1.b)/MAX_COLOUR);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    for (int i=0;i<i1.figures.size();i++)
    {
        fs.push_back(i1.figures[i]);
    }
    sort(fs.begin(),fs.end(),cmpByDepth);
    for (int i=0;i<fs.size();i++)
    {
        curf=fs[i];
        glColor3f(double(curf.r)/MAX_COLOUR, double(curf.g)/MAX_COLOUR, double(curf.b)/MAX_COLOUR);
        glVertex2f((curf.x-double(curf.r1)/2)/MAX_XY*2-1, double(curf.y-double(curf.r2)/2)/MAX_XY*2-1);
        glVertex2f((curf.x+double(curf.r1)/2)/MAX_XY*2-1, double(curf.y-double(curf.r2)/2)/MAX_XY*2-1);
        glVertex2f((curf.x+double(curf.r1)/2)/MAX_XY*2-1, double(curf.y+double(curf.r2)/2)/MAX_XY*2-1);
        glVertex2f((curf.x-double(curf.r1)/2)/MAX_XY*2-1, double(curf.y+double(curf.r2)/2)/MAX_XY*2-1);
    }
    glEnd();
    glfwSwapBuffers(w1);
}
void clearWindow(GLFWwindow* w1)
{
    float ratio;
    int width, height;

    glfwMakeContextCurrent(w1);
    glfwGetFramebufferSize(w1, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    //background
    glColor3f(0, 0, 0);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glEnd();
    glfwSwapBuffers(w1);
}

void draw1Window(GLFWwindow* w1, image i1)
{
    glfwSetWindowShouldClose(w1, 0);
    drawImage(w1,i1);
    while (!glfwWindowShouldClose(w1))
    {
        glfwPollEvents();
    }
}
void draw2Windows(GLFWwindow* w1, image i1, GLFWwindow*  w2, image i2)
{
    glfwSetWindowShouldClose(w1, 0);
    glfwSetWindowShouldClose(w2, 0);
    drawImage(w1,i1);
    drawImage(w2,i2);
    while (!glfwWindowShouldClose(w1) && !glfwWindowShouldClose(w2))
    {
        glfwPollEvents();
    }
}
void draw3Windows(GLFWwindow* w1, image i1, GLFWwindow*  w2, image i2, GLFWwindow*  w3, image i3)
{
    glfwSetWindowShouldClose(w1, 0);
    glfwSetWindowShouldClose(w2, 0);
    glfwSetWindowShouldClose(w3, 0);
    drawImage(w1,i1);
    drawImage(w2,i2);
    drawImage(w3,i3);
    while (!glfwWindowShouldClose(w1) && !glfwWindowShouldClose(w2) && !glfwWindowShouldClose(w3))
    {
        glfwPollEvents();
    }
}

//selection
int compare2Images(GLFWwindow* w1, image i1, GLFWwindow*  w2, image i2)
{
    winner=0;
    drawImage(w1,i1);
    drawImage(w2,i2);
    while (winner==0 && stop==0)
    {
        glfwPollEvents();
    }
    return winner;
}
int compare3Images(GLFWwindow* w1, image i1, GLFWwindow*  w2, image i2, GLFWwindow*  w3, image i3)
{
    winner=0;
    drawImage(w1,i1);
    drawImage(w2,i2);
    drawImage(w3,i3);
    while (winner==0 && stop==0)
    {
        glfwPollEvents();
    }
    return winner;
}
int weakestBetween2Images(image i1, image i2)
{
    int choice,weakest;
    choice=compare2Images(window1,i1,window2,i2);
    if (choice==1) weakest=2;
    else weakest=1;
    return weakest;
}
int weakestBetween3Images(image i1, image i2, image i3)
{
    int choice,weakest;
    if (CHOOSE3==0)
    {
        int wins[3];
        choice=compare2Images(window1,i1,window2,i2);
        if (choice==0) return 0;
        if (choice==1) wins[0]++;
        else wins[1]++;
        choice=compare2Images(window1,i2,window2,i3);
        if (choice==0) return 0;
        if (choice==1) wins[1]++;
        else wins[2]++;
        choice=compare2Images(window1,i3,window2,i1);
        if (choice==0) return 0;
        if (choice==1) wins[2]++;
        else wins[0]++;
        choice=0;
        for (int i=0;i<3;i++)
        {
            if (wins[i]==0) weakest=i+1;
        }
        if (!weakest) weakest==rand()%3+1;
    }
    else
    {
        choice=compare3Images(window1,i1,window2,i2,window3,i3);
        if (choice==0) return 0;
        clearWindow(window3);
        CHOOSE3=0;
        if (choice==1)
        {
            choice=compare2Images(window1,i2,window2,i3);
            if (choice==0) return 0;
            if (choice==1) weakest=3;
            else weakest=2;
        }
        else if (choice==2)
        {
            choice=compare2Images(window1,i1,window2,i3);
            if (choice==0) return 0;
            if (choice==1) weakest=3;
            else weakest=1;
        }
        else if (choice==3)
        {
            choice=compare2Images(window1,i1,window2,i2);
            if (choice==0) return 0;
            if (choice==1) weakest=2;
            else weakest=1;
        }
        CHOOSE3=1;
    }
    return weakest;
}
vector<image> population;
void startSelection()
{
    stop=0;
    int i1,i2,i3;
    int weakest;
    population.resize(MAX_IMAGES);
    for (int i=0;i<MAX_IMAGES;i++)
    {
        population[i].randomise();
    }
    while (stop==0)
    {
        i1=rand()%MAX_IMAGES;
        i2=rand()%(MAX_IMAGES-1);
        i3=rand()%(MAX_IMAGES-3);
        if (i2>=i1) i2++;
        if (i3>=i1)
        {
            i3++;
            if (i3>=i2) i3++;
        }
        else if (i3>=i2)
        {
            i3++;
            if (i3>=i1) i3++;
        }
        weakest=weakestBetween3Images(population[i1],population[i2],population[i3]);
        if (weakest==0) return;
        if (weakest==1)
        {
            population[i1]=population[i2].combine(population[i3]);
        }
        else if (weakest==2)
        {
            population[i2]=population[i1].combine(population[i3]);
        }
        else if (weakest==3)
        {
            population[i3]=population[i1].combine(population[i2]);
        }
    }
}

void setValues()
{
    int a;
    double b;
    cout<<"Do you want to choose between 2 images at a time or between 3?(Enter 2 or 3)\n";
    cin>>a;
    if (a==3) CHOOSE3=1;
    else CHOOSE3=0;
    cout<<"Enter 0 for default settings. Enter 1 to set them manually.\n";
    cin>>a;
    if (a==0) return;
    cout<<"Enter the number of figures.\n";
    cin>>a;
    MAX_FIGURES=a;
    cout<<"Enter the size of the image.\n";
    cin>>a;
    MAX_XY=a+1;
    cout<<"Enter the mutation chance.\n";
    cin>>a;
    MUTATION_CHANCE=a;
    cout<<"Enter the max mutation strength.(double)\n";
    cin>>b;
    MAX_MUTATION_STRENGTH=b;
    cout<<"Keep the figures sorted?(Affects the way the images are crossed)\n0-No.\n1-Only in the beginning.\n2-Yes.\n";
    cin>>a;
    KEEP_SORTED=a;
    cout<<"Keep the depth of each image the same as its size?(Affects the way the images are drawn)\n0-No.\n1-Only in the beginning.\n2-Yes.\n";
    cin>>a;
    KEEP_DEPTH=a;
    cout<<"Enter the size of the population.\n";
    cin>>a;
    MAX_IMAGES=a;
    cout<<"Enter the size of the windows.\n";
    cin>>a;
    WINDOW_SIZE=a;
}

//test
void test()
{
    image a,b,c;
    a.randomise();
    b.randomise();
    int i=0;
    while (stop==0)
    {
        //cout<<i<<endl;
        a.color(255,0,255,1);
        b.color(255,255,0,1);
        c=a.combine(b);
        draw3Windows(window1,a,window2,b,window3,c);
        i++;
    }
}

int main()
{
    srand(time(NULL));
    string message;
    message=initializeGraphics();
    cout<<message<<endl;
    if (message!="Initialized successfully.") return -1;
    setValues();
    message=createWindows();
    cout<<message<<endl;
    if (message!="Windows created successfully.") return -1;
    cout<<"Press Escape to end the program."<<endl;
    int a;
    cout<<"For a demonstration enter 1. Else enter 0."<<endl;
    cin>>a;
    if (a==1 && CHOOSE3==1)
    {
        test();
        stop=0;
    }
    startSelection();
    stopGraphics();
    return 0;
}


