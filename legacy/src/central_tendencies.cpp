#include <vector>

#include <unordered_map>
namespace NNS
{

    template <typename T>
    double median(const std::vector<T> &x)
    {
        size_t l = x.size();
        std::vector<T> temp = x;
        std::sort(temp.begin(), temp.end());
        if (l % 2 == 1)
        {
            return temp[l / 2];
        }
        return (temp[l / 2 - 1] + temp[l / 2]) / 2.0;
    }

    int mode(const std::vector<int> &x)
    {
        std::unordered_map<int, int> freq;
        int mode = x[0];
        int maxCount = 0;

        for (int num : x)
        {
            int count = ++freq[num];
            if (count > maxCount)
            {
                maxCount = count;
                mode = num;
            }
        }

        return mode;
    }

    double mode(const std::vector<double> &x, bool discrete = false, bool multi = true,
                bool skip_unique_check = false)
    {
        int l = x.size();
        if (l <= 3)
        {
            return median(x);
        }
        /*
            Commenting this out for now. Checking for uniqueness is
            expensive, and likely doesn't add much in terms of speed
            at this point in the function
        */
        // if (unique(x).length == 1) {
        //     return x[0];
        // }

        std::vector<double> x_sorted = x;
        std::sort(x_sorted.begin(), x_sorted.end());
        double range = std::abs(x_sorted[l - 1] - x_sorted[0]);
        if (range == 0)
        {
            return x_sorted[0];
        }
        

    }

} // namespace NNS