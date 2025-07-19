#include <opencv2/opencv.hpp>
#include <opencv2/photo.hpp>
#include <iostream>
#include <chrono>
#include <string>

class VideoEnhancer {
private:
    cv::VideoCapture cap;
    cv::VideoWriter writer;
    cv::Size frameSize;
    double fps;
    
    int h = 10;
    int templateWindowSize = 7;
    int searchWindowSize = 21;
    double alpha = 1.2;
    int beta = 10;
    
public:
    bool initialize(const std::string& input, const std::string& output) {
        cap.open(input);
        if (!cap.isOpened()) {
            std::cerr << "Cannot open input: " << input << std::endl;
            return false;
        }
        
        frameSize = cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), 
                           cap.get(cv::CAP_PROP_FRAME_HEIGHT));
        fps = cap.get(cv::CAP_PROP_FPS);
        
        writer.open(output, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, frameSize, true);
        if (!writer.isOpened()) {
            std::cerr << "Cannot open output: " << output << std::endl;
            return false;
        }
        
        return true;
    }
    
    cv::Mat denoise(const cv::Mat& frame) {
        cv::Mat denoised;
        cv::fastNlMeansDenoisingColored(frame, denoised, h, h, 
                                       templateWindowSize, searchWindowSize);
        return denoised;
    }
    
    cv::Mat enhanceContrast(const cv::Mat& frame) {
        cv::Mat lab, enhanced;
        cv::cvtColor(frame, lab, cv::COLOR_BGR2LAB);
        
        std::vector<cv::Mat> planes;
        cv::split(lab, planes);
        
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(2.0);
        clahe->setTilesGridSize(cv::Size(8, 8));
        clahe->apply(planes[0], planes[0]);
        
        cv::merge(planes, lab);
        cv::cvtColor(lab, enhanced, cv::COLOR_LAB2BGR);
        enhanced.convertTo(enhanced, -1, alpha, beta);
        
        return enhanced;
    }
    
    void process() {
        cv::Mat frame, denoised, enhanced;
        int frameCount = 0;
        auto start = std::chrono::high_resolution_clock::now();
        
        while (cap >> frame) {
            denoised = denoise(frame);
            enhanced = enhanceContrast(denoised);
            writer.write(enhanced);
            frameCount++;
            
            if (frameCount % 100 == 0) {
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
                std::cout << "Processed " << frameCount << " frames in " 
                         << elapsed.count() << "s" << std::endl;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto total = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        
        std::cout << "Completed " << frameCount << " frames in " 
                 << total.count() << " seconds" << std::endl;
    }
    
    void setParams(int h_val, double alpha_val, int beta_val) {
        h = h_val;
        alpha = alpha_val;
        beta = beta_val;
    }
    
    ~VideoEnhancer() {
        if (cap.isOpened()) cap.release();
        if (writer.isOpened()) writer.release();
    }
};

int main(int argc, char** argv) {
    std::string input = "input.mp4";
    std::string output = "output.avi";
    
    if (argc >= 2) input = argv[1];
    if (argc >= 3) output = argv[2];
    
    VideoEnhancer enhancer;
    
    if (!enhancer.initialize(input, output)) {
        return -1;
    }
    
    enhancer.setParams(10, 1.2, 10);
    enhancer.process();
    
    return 0;
}