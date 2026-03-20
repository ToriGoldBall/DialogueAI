#include "addcharacterdialog.h"
#include "ui_AddCharacterDialog.h"

#include <QFileDialog>
//#include <algorithm>
//#include <set>

//namespace
//{
    //set<set<int>> recurse (long& currentMax, set<int>& combo, const size_t& depth)
    //{
    //    size_t cmbSize = combo.size();
    //    if(depth == cmbSize-1)
    //    {
    //        long val = INT64_MAX;
    //        for(size_t i = 0; i<cmbSize-1; ++i)
    //        {
    //            val = min(val, static_cast<long>(abs(combo[i] - combo[i + 1])));
    //            if(val == 1 || val < currentMax)
    //            {
    //                break;
    //            }
    //        }
    //        if(val >= currentMax)
    //        {
    //            currentMax = val;
    //            return {combo};
    //        }
    //        return {};
    //    }
    //    int lastMax = currentMax;
    //    set<set<int>> results;
    //    set<int> copyCombo = combo;
    //    for(size_t i = depth; i<combo.size(); ++i)
    //    {
    //        //move the variable at i to the depth, if i == depth don't bother
    //        if(i != depth)
    //        {
    //            int valToMove = copyCombo[i];
    //            copyCombo.erase(std::begin(copyCombo) + i);
    //            copyCombo.emplace(std::begin(copyCombo)+depth, valToMove);
    //        }

    //        set<set<int>> recResults = recurse(currentMax, copyCombo, depth+1);

    //        if(lastMax != currentMax)
    //        {
    //            results = recResults;
    //            lastMax = currentMax;
    //        }
    //        else
    //        {
    //            for(set<int> result : recResults)
    //            {
    //                results.emplace_back(result);
    //            }
    //        }
    //        copyCombo = combo;
    //    }
    //    return results;
    //}


    //vector<int> solve(int n, long k)
    //{
    //    set<int> src(n,1);
    //    for(int i=1; i<n; ++i)
    //    {
    //        auto iter = src.begin();
    //        std::advance(iter, i);
    //        iter. = i+1;
    //    }
    //    long max = 0;
    //    set<set<int>> maxResults = recurse(max, src, 0);

    //    if(k <= maxResults.size())
    //    {
    //        vector<int> payload(n,0);
    //        auto iter = maxResults.begin();
    //
    //        for(int index : maxResults.[k-1])
    //        {
    //            payload.emplace_back((index);)
    //        }
    //        return payload;
    //    }
    //    else
    //    {
    //        return {-1};
    //    }
    //}

    //vector<int> pathFind(vector<bool> half1, vector<bool> half2, int maxDist, long k)
    //{
    //    size_t numOptions = std::size(options);
    //    int startVal = 1;
    //    bool goToHigh = false;
    //    vector<int> potential;
    //    while(static_cast<size_t>(startVal) <= numOptions)
    //    {
    //        for(int currentVal = startVal;
    //            std::accumulate(std::begin(options), std::end(options),false);
    //            currentVal += goToHigh? maxDist : (-maxDist + 1))
    //        {
    //            if(potential.empty() || (std::abs(potential.back() - currentVal) >= maxDist))
    //            {
    //                options[currentVal-1] = false;
    //                potential.emplace_back(currentVal);
    //                if(!std::accumulate(std::begin(options), std::end(options),false))
    //                {
    //                    k--;
    //                    if(k == 0)
    //                    {
    //                        vector<int> soln;
    //                        for(size_t t : potential)
    //                        {
    //                            soln.emplace_back(static_cast<int>(t));
    //                        }
    //                        return soln;
    //                    }
    //                }
    //                goToHigh = !goToHigh;
    //            }
    //            else
    //            {
    //                break;
    //            }
    //        }
    //        //reset everything
    //        potential.clear();
    //        for(size_t i=0; i<std::size(options); ++i)
    //        {
    //            options[i] = true;
    //        }
    //        ++startVal;
    //    }
    //    return {-1};
    //}

//int fact(int curr)
//{
//    if(curr <= 0)
//    {
//        return 0;
//    }
//    if(curr == 1)
//    {
//        return 1;
//    }
//    else
//    {
//        return curr * fact(curr-1);
//    }
//}
//
//    vector<int> solve(int n, long k)
//    {
//        if(n==1)
//        {
//            return k == 1? std::vector<int>{1} : std::vector<int>{-1};
//        }
//        int maxDist = static_cast<int>(floor(n/2.0));
//        //vector<int8_t> half1(maxDist == 1 ? 1 : maxDist-1, 1);
//        //vector<int8_t> half2(n - maxDist, 1);
//
//        vector<int> options(n, 1);
//
//        std::vector<int> soln;
//        bool firstHalf = true;
//        int iterator = 0;
//        while(soln.size() < n)
//        {
//            //calculate the block size out of what is left
//            int blockSize = fact(std::accumulate(begin(options), begin(options) + maxDist - 1, 0)) * fact(std::accumulate(begin(options) + maxDist, end(options), 0));
//
//            //if we need to move the block forward, subtract the blocksize from k
//            while(k > blockSize)
//            {
//                //move the iterator forward to skip the block and subtract blocksize from k
//                iterator += blockSize;
//                if(iterator >= std::size(options))
//                {
//                    iterator = 0;
//                }
//                k -= blockSize;
//            }
//            //if we should be within this block, add the current value, subtract 1 from k
//            else if (k <= blockSize)
//            {
//                //choose the current one, iterate, subtract 1 from k
//                soln.emplace_back(iterator+1);
//                options[firstHalf ? iterator : (iterator + maxDist)] = 0;
//                iterator++;
//                //if we need to loop back, loop back
//                if(iterator >= maxDist)
//                {
//                    iterator = 0;
//                }
//
//                k--; //getting closer by 1 step
//            }
//            else
//            {
//                return {-1};
//            }
//
//            //jump by the most amount you can for the current block size
//            //
//            //if jumping would fail
//            //  return {-1}
//
//            //emplace the current spot
//            //soln.emplace_back();
//        }
//        return soln.size() == n ? soln : {-1};
//
//        //int blockJumpSize = fact(static_cast<int>(std::size(half1))) * fact(static_cast<int>(std::size(half2)));
//        //bool isLowStart = true;
//
//        //vector<int> solution;
//        //while(solution.size() < n)
//        //{
//        //    if(isLowStart)
//        //    {
//        //        if(half1[index1])
//        //        {
//        //            half1[index1] = false;
//        //            solution.emplace_back(index1+1);
//        //            index1++;
//        //            if(index1 >= std::size(half1))
//        //            {
//        //                index1 = 0;
//        //            }
//        //        }
//        //    }
//        //    else
//        //    {
//        //        if(half2[index2])
//        //        {
//        //            half2[index2] = false;
//        //            solution.emplace_back(index2 + std::size(half1) + 1);
//        //            index2++;
//        //            if(index2 >= std::size(half2))
//        //            {
//        //                index2 = 0;
//        //            }
//        //        }
//        //    }
//        //    isLowStart = ! isLowStart;
//        //}
//        //return solution;
//    }
//}




AddCharacterDialog::AddCharacterDialog(QWidget *parent) : QDialog(parent)
{
    ui = std::make_unique<Ui::AddCharacterDialog>();
    ui->setupUi(this);

    //disable the ok button up front
    ui->buttonBox->setEnabled(false);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(ui->CharacterNameEdit, &QPlainTextEdit::textChanged, this, &AddCharacterDialog::OnCharNameChanged);
    connect(ui->CharacterBioEdit, &QPlainTextEdit::textChanged, this, &AddCharacterDialog::OnCharBioChanged);

    ui->CharacterImageEdit->setScene(new QGraphicsScene(this));
    connect(ui->CharacterImageEdit, &ClickableGraphicsView::clicked, this, &AddCharacterDialog::OnCharImageClicked);
    connect(ui->CharacterImageEdit, &ClickableGraphicsView::imageDropped, this, &AddCharacterDialog::OnCharImageDropped);
}

AddCharacterDialog::~AddCharacterDialog()
{
}

void AddCharacterDialog::OnCharNameChanged()
{
    m_charName = ui->CharacterNameEdit->toPlainText();
    validate();

    /////////////////////////////MAX HEAP

    //std::vector<int> someInts {0,1,2,3,4,5,6};
    //std::make_heap(someInts.begin(), someInts.end());
    //std::pop_heap(someInts.begin(), someInts.end());
    //auto topMax = someInts.back();
    //someInts.pop_back();

    /////////////////////////////MIN HEAP

    //someInts = {0,1,2,3,4,5,6};
    //std::make_heap(someInts.begin(), someInts.end(), std::greater<>{});
    //std::pop_heap(someInts.begin(), someInts.end(), std::greater<>{});
    //auto topMin = someInts.back();
    //someInts.pop_back();



    ///////////////////////////////////////
    //std::vector<int> res1 = solve(3, 5);    //3,1,2
    //std::vector<int> res2 = solve(4, 2);    //3,1,4,2
    //std::vector<int> res3 = solve(4, 3);    //-1

    //std::vector<int> res4 = solve(500000, 1);    //-1
    //std::vector<int> res5 = solve(500000, 2);    //-1

    ///////////////////////////////////////
}

void AddCharacterDialog::OnCharBioChanged()
{
    m_charBio = ui->CharacterBioEdit->toPlainText();
    validate();

    //std::unordered_map<int, char> buses {{ 1, 'B'}, {2, 'E'} };

    //BusTracker busTracker(buses);

    //int res1 = busTracker.nearestBusToStation("A").value(); // -1
    //int res2 = busTracker.nearestBusToStation("B").value(); // 0
    //int res3 = busTracker.nearestBusToStation("C").value(); // 1
    //int res4 = busTracker.nearestBusToStation("D").value(); // 2
    //int res5 = busTracker.nearestBusToStation("E").value(); // 0
    //int res6 = busTracker.nearestBusToStation("F").value(); // 1
    //int res7 = busTracker.nearestBusToStation("G").value(); // 2
    //int res8 = busTracker.nearestBusToStation("H").value(); // 3
    //int res9 = busTracker.nearestBusToStation("I").value(); // 4
    //char ch1 = busTracker.getBusLocation(1).value(); // B
    //char ch2 = busTracker.getBusLocation(2).value(); // E
}

void AddCharacterDialog::OnCharImageClicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select Image"), QString(), tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if(file.isEmpty())
    {
        return;
    }
    QPixmap pix(file);
    if(pix.isNull())
    {
        return;
    }
    SetPixmap(pix, file);
}

void AddCharacterDialog::OnCharImageDropped(const QString& path)
{
    QPixmap pix(path);
    if(pix.isNull())
    {
        return;
    }
    SetPixmap(pix, path);
}

void AddCharacterDialog::SetPixmap(QPixmap& pix, const QString& path)
{
    ui->CharacterImageEdit->scene()->clear();
    ui->CharacterImageEdit->scene()->addPixmap(pix);
    ui->CharacterImageEdit->fitInView(
            ui->CharacterImageEdit->scene()->itemsBoundingRect(),
            Qt::KeepAspectRatio
            );

    m_imagePath = path;
    validate();
}

void AddCharacterDialog::validate()
{
    bool okEnabled = true;
    if(m_charName.isEmpty() || m_charBio.isEmpty() || m_imagePath.isEmpty())
    {
        okEnabled = false;
    }
    ui->buttonBox->setEnabled(okEnabled);

}
