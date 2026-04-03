#pragma once

namespace application {

/**
 * Base interface for all simulation commands.
 * Commands represent intentions that alter the simulation state.
 */
struct ICommand {
    virtual ~ICommand() = default;
    
    // In the future, we could add methods like virtual void Execute(...) = 0;
};

} // namespace application
