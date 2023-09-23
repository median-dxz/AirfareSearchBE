/**
 *
 * @param {number} min
 * @param {number} max
 */
export function getRand(min, max) {
    min = Math.trunc(min);
    max = Math.trunc(max);
    return Math.round(Math.random() * (max - min) + min);
}
