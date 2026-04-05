// PerfMeter.hpp (header-only sencillo)
#pragma once
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <string>
#include <cstdint>
#include <fstream>
#include <atomic>

class PerfMeter {
public:
    struct Stat {
        std::chrono::steady_clock::time_point lastRx{};
        uint64_t updates = 0;
        double emaMs = 0.0;     // media móvil exponencial de inter-arrival
        uint64_t lastDtMs = 0;  // último dt calculado
    };

    explicit PerfMeter(const std::string& csvPath = "aircraft_updates.csv")
        : _csvPath(csvPath)
    {
        // cabecera CSV
        std::ofstream f(_csvPath, std::ios::app);
        if (f.tellp() == 0) {
            f << "epoch_ms,instance,updates,dt_ms,ema_ms,num_attrs,producer\n";
        }
    }

    // Llamar en cada attributesUpdated
    template<typename StringLike>
    void onUpdate(const StringLike& instanceName,
                  size_t numAttrs,
                  const std::string& producerFederate)
    {
        using namespace std::chrono;
        const auto now = steady_clock::now();
        const auto epochMs = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()).count();

        std::lock_guard<std::mutex> lk(_mtx);
        auto& s = _byInstance[std::wstring(instanceName.begin(), instanceName.end())];

        uint64_t dtMs = 0;
        if (s.updates > 0) {
            dtMs = (uint64_t)duration_cast<milliseconds>(now - s.lastRx).count();
            // EMA con alpha=0.1 (ajusta a tu gusto)
            s.emaMs = (s.emaMs == 0.0) ? (double)dtMs : (0.9 * s.emaMs + 0.1 * dtMs);
            s.lastDtMs = dtMs;
        }
        s.lastRx = now;
        s.updates++;

        // Log CSV (barato y legible; para alto throughput, usa un buffer/cola)
        std::ofstream f(_csvPath, std::ios::app);
        f << epochMs << ","
          << toUtf8(instanceName) << ","
          << s.updates << ","
          << dtMs << ","
          << s.emaMs << ","
          << numAttrs << ","
          << producerFederate << "\n";
    }

    // Opcional: limpiar métricas al hacer deleted
    template<typename StringLike>
    void onDelete(const StringLike& instanceName) {
        std::lock_guard<std::mutex> lk(_mtx);
        _byInstance.erase(std::wstring(instanceName.begin(), instanceName.end()));
    }

private:
    static std::string toUtf8(const std::wstring& ws) {
        // conversión mínima; si ya usas utilidades propias, reemplázala
        std::string s; s.reserve(ws.size());
        for (wchar_t c : ws) s.push_back((c <= 0x7F) ? char(c) : '?');
        return s;
    }

    std::mutex _mtx;
    std::unordered_map<std::wstring, Stat> _byInstance;
    std::string _csvPath;
};
